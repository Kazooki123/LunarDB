use thiserror::Error;

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

pub struct LunarAI {
    embeddings: embeddings::EmbeddingEngine,
    rag: rag::RAGEngine,
}

impl LunarAI {
    pub fn new() -> Self {
        Self {
            embeddings: embeddings::EmbeddingEngine::new(),
            rag: rag::RAGEngine::new(),
        }
    }

    pub fn version() -> &'static str {
        env!("CARGO_PKG_VERSION")
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_lunar_ai_creation() {
        let lunar_ai = LunarAI::new();
        assert_eq!(LunarAI::version(), env!("CARGO_PKG_VERSION"));
    }
}
