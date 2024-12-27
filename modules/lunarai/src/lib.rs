use core::mem::drop;
use core::option::Option::{None, Some};
use core::result::Result::Ok;
use std::ffi::{c_char, CStr};
use std::path::PathBuf;
use std::ptr;

pub mod ai;
pub mod embeddings;
pub mod models;
pub mod rag;

pub use ai::{AIConfig, AIPipeline};
pub use embeddings::{Embedding, EmbeddingError};
pub use models::ModelManager;
pub use rag::{RAGSystem, Document};

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
pub struct LunarAI {
    pipeline: AIPipeline,
    rag_system: Option<RAGSystem>,
}

impl LunarAI {
    pub fn new(config: AIConfig) -> Self {
        let pipeline = AIPipeline::new(config);
        Self {
            pipeline,
            rag_system: None,
        }
    }

    pub async fn initialize(&mut self) -> Result<(), EmbeddingError> {
        self.pipeline.initialize().await?;
        self.rag_system = Some(RAGSystem::new(self.pipeline.clone()));
        Ok(())
    }
}

#[no_mangle]
pub extern "C" fn lunar_ai_new(model_path: *const c_char) -> *mut LunarAI {
    let path_str = unsafe {
        if model_path.is_null() {
            return ptr::null_mut();
        }
        match CStr::from_ptr(model_path).to_str() {
            Ok(s) => s,
            Err(_) => return ptr::null_mut(),
        }
    };

    let config = AIConfig {
        model_path: PathBuf::from(path_str),
        ..Default::default()
    };

    let ai = Box::new(LunarAI::new(config));
    Box::into_raw(ai)
}

#[no_mangle]
pub unsafe extern "C" fn lunar_ai_free(ptr: *mut LunarAI) {
    if !ptr.is_null() {
        drop(Box::from_raw(ptr));
    }
}

#[no_mangle]
pub unsafe extern "C" fn lunar_ai_generate_embedding(
    ptr: *mut LunarAI,
    text: *const c_char,
    error: *mut FFIError,
) -> *mut Embedding {
    if ptr.is_null() || text.is_null() || error.is_null() {
        if !error.is_null() {
            *error = FFIError::NullPointer;
        }
        return ptr::null_mut();
    }

    let ai = &mut *ptr;
    let text_str = match CStr::from_ptr(text).to_str() {
        Ok(s) => s,
        Err(_) => {
            *error = FFIError::InvalidUtf8;
            return ptr::null_mut();
        }
    };

    let rt = tokio::runtime::Runtime::new().unwrap();
    let embedding_result = rt.block_on(async {
        ai.pipeline.generate_embedding(text_str).await
    });

    match embedding_result {
        Ok(embedding) => {
            *error = FFIError::Success;
            Box::into_raw(Box::new(embedding))
        }
        Err(_) => {
            *error = FFIError::EmbeddingError;
            ptr::null_mut()
        }
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[tokio::test]
    async fn test_full_pipeline() {
        let config = AIConfig::default();
        let mut ai = LunarAI::new(config);

        assert!(ai.initialize().await.is_ok());

        let result = ai.pipeline.generate_embedding("Test text").await;
        assert!(result.is_ok());
    }
}
