use thiserror::Error;
use std::ffi::{CStr, CString};
use std::os::raw::{c_char, c_float, c_int};
use std::slice;

pub mod embeddings;
pub mod rag;
pub mod tokenizer;
pub mod models;
pub mod storage;

#[derive(Error, Debug)]
pub enum LunarAIError {
    #[error("Invalid input dimensions: {0}")]
    InvalidDimensions(String),

    #[error("Model not found: {0}")]
    ModelNotFound(String),

    #[error("Storage error: {0}")]
    StorageError(String),

    #[error("Embedding error: {0}")]
    EmbeddingError(String),

    #[error("RAG error: {0}")]
    RAGError(String),
}

pub type Result<T> = std::result::Result<T, LunarAIError>;

// Main LunarAI struct
pub struct LunarAI {
    embeddings: embeddings::EmbeddingEngine,
    rag: rag::RAGEngine,
    tokenizer: tokenizer::Tokenizer,
}

// Implementation visible to Rust
impl LunarAI {
    pub fn new() -> Self {
        Self {
            embeddings: embeddings::EmbeddingEngine::new(),
            rag: rag::RAGEngine::new(),
            tokenizer: tokenizer::Tokenizer::new(),
        }
    }

    pub fn version() -> &'static str {
        env!("CARGO_PKG_VERSION")
    }
}

// FFI-safe struct for C++
#[repr(C)]
pub struct LunarAIHandle(*mut LunarAI);

// Error codes for C++ interface
#[repr(C)]
#[derive(Debug, PartialEq)]
pub enum LunarAIStatusCode {
    Success = 0,
    InvalidInput = 1,
    ModelError = 2,
    StorageError = 3,
    EmbeddingError = 4,
    RAGError = 5,
    UnknownError = 99,
}

// C++ compatible functions
#[no_mangle]
pub extern "C" fn lunar_ai_create() -> LunarAIHandle {
    let lunar_ai = Box::new(LunarAI::new());
    LunarAIHandle(Box::into_raw(lunar_ai))
}

#[no_mangle]
pub extern "C" fn lunar_ai_destroy(handle: LunarAIHandle) {
    if !handle.0.is_null() {
        unsafe {
            drop(Box::from_raw(handle.0));
        }
    }
}

#[no_mangle]
pub extern "C" fn lunar_ai_version() -> *const c_char {
    let version = CString::new(LunarAI::version()).unwrap();
    version.into_raw()
}

#[no_mangle]
pub extern "C" fn lunar_ai_generate_embedding(
    handle: LunarAIHandle,
    text: *const c_char,
    embedding_out: *mut c_float,
    embedding_size: *mut c_int,
) -> LunarAIStatusCode {
    if handle.0.is_null() || text.is_null() || embedding_out.is_null() || embedding_size.is_null() {
        return LunarAIStatusCode::InvalidInput;
    }

    unsafe {
        let lunar_ai = &*handle.0;
        let text_str = match CStr::from_ptr(text).to_str() {
            Ok(s) => s,
            Err(_) => return LunarAIStatusCode::InvalidInput,
        };

        match lunar_ai.embeddings.generate_embedding(text_str) {
            Ok(embedding) => {
                *embedding_size = embedding.len() as c_int;
                let embedding_slice = slice::from_raw_parts_mut(embedding_out, embedding.len());
                embedding_slice.copy_from_slice(&embedding);
                LunarAIStatusCode::Success
            }
            Err(_) => LunarAIStatusCode::EmbeddingError,
        }
    }
}

#[no_mangle]
pub extern "C" fn lunar_ai_compute_similarity(
    handle: LunarAIHandle,
    embedding1: *const c_float,
    embedding2: *const c_float,
    size: c_int,
    similarity_out: *mut c_float,
) -> LunarAIStatusCode {
    if handle.0.is_null() || embedding1.is_null() || embedding2.is_null() || similarity_out.is_null() {
        return LunarAIStatusCode::InvalidInput;
    }

    unsafe {
        let lunar_ai = &*handle.0;
        let emb1 = slice::from_raw_parts(embedding1, size as usize);
        let emb2 = slice::from_raw_parts(embedding2, size as usize);

        match lunar_ai.embeddings.similarity(emb1, emb2) {
            Ok(similarity) => {
                *similarity_out = similarity;
                LunarAIStatusCode::Success
            }
            Err(_) => LunarAIStatusCode::EmbeddingError,
        }
    }
}

#[no_mangle]
pub extern "C" fn lunar_ai_free_string(ptr: *mut c_char) {
    unsafe {
        if !ptr.is_null() {
            drop(CString::from_raw(ptr));
        }
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_ffi_creation_destruction() {
        let handle = lunar_ai_create();
        assert!(!handle.0.is_null());
        lunar_ai_destroy(handle);
    }

    #[test]
    fn test_ffi_version() {
        let version_ptr = lunar_ai_version();
        unsafe {
            let version = CStr::from_ptr(version_ptr).to_str().unwrap();
            assert_eq!(version, LunarAI::version());
            lunar_ai_free_string(version_ptr as *mut c_char);
        }
    }
}
