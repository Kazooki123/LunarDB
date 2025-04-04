use std::collections::HashMap;
use std::ffi::{CStr, CString};
use std::os::raw::{c_char, c_float, c_int};
use std::sync::Mutex;
use std::time::Duration;

mod vector;

use vector::LunarVector;

// Global state wrapped in a mutex for thread safety
lazy_static::lazy_static! {
    static ref VECTOR_DBS: Mutex<HashMap<String, LunarVector>> = Mutex::new(HashMap::new());
}

#[no_mangle]
pub extern "C" fn lunar_vector_create(db_key: *const c_char) -> c_int {
    let db_key = unsafe {
        match CStr::from_ptr(db_key).to_str() {
            Ok(s) => s.to_string(),
            Err(_) => return 0,
        }
    };

    let mut dbs = VECTOR_DBS.lock().unwrap();
    dbs.insert(db_key, LunarVector::new());
    1
}

#[no_mangle]
pub extern "C" fn lunar_vector_add(
    db_key: *const c_char,
    vector_id: *const c_char,
    data: *const c_float,
    data_len: c_int,
) -> c_int {
    let db_key = unsafe {
        match CStr::from_ptr(db_key).to_str() {
            Ok(s) => s.to_string(),
            Err(_) => return 0,
        }
    };

    let vector_id = unsafe {
        match CStr::from_ptr(vector_id).to_str() {
            Ok(s) => s.to_string(),
            Err(_) => return 0,
        }
    };

    let data = unsafe {
        std::slice::from_raw_parts(data, data_len as usize)
            .iter()
            .copied()
            .collect::<Vec<f32>>()
    };

    let mut dbs = VECTOR_DBS.lock().unwrap();
    match dbs.get_mut(&db_key) {
        Some(db) => {
            db.add(vector_id, data);
            1
        }
        None => 0,
    }
}

#[no_mangle]
pub extern "C" fn lunar_vector_search(
    db_key: *const c_char,
    query: *const c_float,
    query_len: c_int,
    limit: c_int,
    results: *mut *mut c_char,
    scores: *mut c_float,
) -> c_int {
    let db_key = unsafe {
        match CStr::from_ptr(db_key).to_str() {
            Ok(s) => s.to_string(),
            Err(_) => return 0,
        }
    };

    let query = unsafe {
        std::slice::from_raw_parts(query, query_len as usize)
    };

    let dbs = VECTOR_DBS.lock().unwrap();
    match dbs.get(&db_key) {
        Some(db) => {
            let similar = db.search_similar(query, limit as usize);
            for (i, (id, score)) in similar.iter().enumerate() {
                unsafe {
                    let c_id = CString::new(id.clone()).unwrap();
                    *results.add(i) = c_id.into_raw();
                    *scores.add(i) = score.clone();
                }
            }
            similar.len() as c_int
        }
        None => 0,
    }
}

#[no_mangle]
pub extern "C" fn lunar_vector_set_ttl(
    db_key: *const c_char,
    vector_id: *const c_char,
    ttl_seconds: c_int,
) -> c_int {
    let db_key = unsafe {
        match CStr::from_ptr(db_key).to_str() {
            Ok(s) => s.to_string(),
            Err(_) => return 0,
        }
    };

    let vector_id = unsafe {
        match CStr::from_ptr(vector_id).to_str() {
            Ok(s) => s.to_string(),
            Err(_) => return 0,
        }
    };

    let mut dbs = VECTOR_DBS.lock().unwrap();
    match dbs.get_mut(&db_key) {
        Some(db) => {
            if db.set_ttl(&vector_id, Duration::from_secs(ttl_seconds as u64)) {
                1
            } else {
                0
            }
        }
        None => 0,
    }
}

#[no_mangle]
pub extern "C" fn lunar_vector_cleanup() {
    let mut dbs = VECTOR_DBS.lock().unwrap();
    for db in dbs.values_mut() {
        db.cleanup_expired();
    }
}
