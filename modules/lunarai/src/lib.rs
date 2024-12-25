use core::mem::drop;
use core::option::Option::{None, Some};
use core::result::Result::Ok;
use std::ffi::{c_char, CStr, CString};
use std::ptr;

pub mod ai;
pub mod embeddings;
pub mod models;
pub mod rag;

pub use ai::{AIConfig, AIPipeline, AIError};
pub use embeddings::{Embedding, EmbeddingError};
pub use models::Model;
pub use rag::RAGSystem;

#[repr(C)]
pub enum FFIError {
    Success = 0,
    EmbeddingError = 1,
    ModelError = 2,
    RAGError = 3,
    NullPointer = 4,
    InvalidUtf8 = 5,
}

#[repr(C)]
pub struct LunarEmbeddings {
    pipeline: AIPipeline,
    rag_system: Option<RAGSystem>,
}

impl LunarEmbeddings {
    pub fn new(config: AIConfig) -> Self {
        Self {
            pipeline: AIPipeline::new(config),
            rag_system: None,
        }
    }

    pub async fn initialize_rag(&mut self, model_name: &str) -> Result<(), AIError> {
        let model = Model::new(model_name)?;
        self.rag_system = Some(RAGSystem::new(model));
        Ok(())
    }
}

#[no_mangle]
pub extern "C" fn lunar_embeddings_new() -> *mut LunarEmbeddings {
    let config = AIConfig::default();
    let embeddings = Box::new(LunarEmbeddings::new(config));
    Box::into_raw(embeddings)
}

#[no_mangle]
pub extern "C" fn lunar_embeddings_free(ptr: *mut LunarEmbeddings) {
    if !ptr.is_null() {
        unsafe {
            drop(Box::from_raw(ptr));
        }
    }
}

#[no_mangle]
pub extern "C" fn lunar_embeddings_generate(
    ptr: *mut LunarEmbeddings,
    text: *const c_char,
    model_name: *const c_char,
    error: *mut FFIError,
) -> *mut Embedding {
    if ptr.is_null() || text.is_null() {
        unsafe { *error = FFIError::NullPointer; }
        return ptr::null_mut();
    }

    let embeddings = unsafe { &mut *ptr };
    let text_str = unsafe {
        match CStr::from_ptr(text).to_str() {
            Ok(s) => s,
            Err(_) => {
                *error = FFIError::InvalidUtf8;
                return ptr::null_mut();
            }
        }
    };

    let model_name_str = if model_name.is_null() {
        None
    } else {
        Some(unsafe {
            match CStr::from_ptr(model_name).to_str() {
                Ok(s) => s.to_string(),
                Err(_) => {
                    *error = FFIError::InvalidUtf8;
                    return ptr::null_mut();
                }
            }
        })
    };

    // Create a runtime for async operations
    let rt = tokio::runtime::Runtime::new().unwrap();
    let embedding_result = rt.block_on(async {
        embeddings.pipeline.generate_embedding(text_str, model_name_str).await
    });

    match embedding_result {
        Ok(embedding) => {
            unsafe { *error = FFIError::Success; }
            Box::into_raw(Box::new(embedding))
        }
        Err(_) => {
            unsafe { *error = FFIError::EmbeddingError; }
            ptr::null_mut()
        }
    }
}

#[no_mangle]
pub extern "C" fn lunar_embeddings_initialize_rag(
    ptr: *mut LunarEmbeddings,
    model_name: *const c_char,
    error: *mut FFIError,
) -> bool {
    if ptr.is_null() || model_name.is_null() {
        unsafe { *error = FFIError::NullPointer; }
        return false;
    }

    let embeddings = unsafe { &mut *ptr };
    let model_name_str = unsafe {
        match CStr::from_ptr(model_name).to_str() {
            Ok(s) => s,
            Err(_) => {
                *error = FFIError::InvalidUtf8;
                return false;
            }
        }
    };

    let rt = tokio::runtime::Runtime::new().unwrap();
    match rt.block_on(embeddings.initialize_rag(model_name_str)) {
        Ok(_) => {
            unsafe { *error = FFIError::Success; }
            true
        }
        Err(_) => {
            unsafe { *error = FFIError::RAGError; }
            false
        }
    }
}

#[no_mangle]
pub extern "C" fn lunar_embeddings_string_free(s: *mut c_char) {
    unsafe {
        if !s.is_null() {
            drop(CString::from_raw(s));
        }
    }
}

#[no_mangle]
pub extern "C" fn lunar_embeddings_get_version() -> *mut c_char {
    let version = CString::new("1.0.0").unwrap();
    version.into_raw()
}

#[cfg(test)]
mod tests {
    use core::option::Option::None;

    use super::*;

    #[test]
    fn test_create_and_free() {
        let embeddings = lunar_embeddings_new();
        assert!(!embeddings.is_null());
        lunar_embeddings_free(embeddings);
    }

    #[tokio::test]
    async fn test_embedding_generation() {
        let mut embeddings = LunarEmbeddings::new(AIConfig::default());
        let result = embeddings.pipeline.generate_embedding(
            "Test text",
            None
        ).await;
        assert!(result.is_ok());
    }
}
