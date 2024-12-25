use crate::stream::Stream;
use std::ffi::{CString, CStr};
use std::os::raw::c_char;

#[no_mangle]
pub extern "C" fn stream_new(name: *const c_char) -> *mut Stream {
    let name = unsafe {
        assert!(!name.is_null());
        CStr::from_ptr(name)
    };

    let stream = Stream::new(name.to_str().unwrap());
    Box::into_raw(Box::new(stream))
}

#[no_mangle]
pub extern "C" fn stream_add_message(stream: *mut Stream, payload: *const c_char) -> *mut c_char {
    let stream = unsafe {
        assert!(!stream.is_null());
        &mut *stream
    };

    let payload = unsafe {
        assert!(!payload.is_null());
        CStr::from_ptr(payload)
    };

    let id = stream.add_message(payload.to_str().unwrap());
    CString::new(id).unwrap().into_raw()
}

#[no_mangle]
pub extern "C" fn stream_free(stream: *mut Stream) {
    if !stream.is_null() {
        unsafe { Box::from_raw(stream) };
    }
}

#[no_mangle]
pub extern "C" fn string_free(s: *mut c_char) {
    if !s.is_null() {
        unsafe { CString::from_raw(s) };
    }
}
