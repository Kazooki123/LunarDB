use crate::spatial::{Point, Region, SpatialIndex};
use std::ffi::CString;
use std::os::raw::c_char;

#[no_mangle]
pub extern "C" fn create_spatial_index() -> *mut SpatialIndex {
    Box::into_raw(Box::new(SpatialIndex::new()))
}

#[no_mangle]
pub extern "C" fn add_region_to_index(
    index_ptr: *mut SpatialIndex,
    x1: f64,
    y1: f64,
    x2: f64,
    y2: f64,
) {
    let index = unsafe {
        assert!(!index_ptr.is_null());
        &mut *index_ptr
    };

    index.add_region(Region::new(Point::new(x1, y1), Point::new(x2, y2)));
}

#[no_mangle]
pub extern "C" fn find_points_in_region(
    index_ptr: *mut SpatialIndex,
    x1: f64,
    y1: f64,
    x2: f64,
    y2: f64,
) -> *mut c_char {
    let index = unsafe {
        assert!(!index_ptr.is_null());
        &*index_ptr
    };

    let region = Region::new(Point::new(x1, y1), Point::new(x2, y2));
    let points = index
        .find_points_in_region(&region)
        .iter()
        .map(|p| format!("({}, {})", p.x, p.y))
        .collect::<Vec<_>>()
        .join(", ");
    CString::new(points).unwrap().into_raw()
}

#[no_mangle]
pub extern "C" fn free_spatial_index(index_ptr: *mut SpatialIndex) {
    if index_ptr.is_null() {
        return;
    }
    unsafe {
        Box::from_raw(index_ptr);
    }
}

#[no_mangle]
pub extern "C" fn free_string(s: *mut c_char) {
    if s.is_null() {
        return;
    }
    unsafe {
        CString::from_raw(s);
    }
}
