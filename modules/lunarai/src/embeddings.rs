use ndarray::Array1;
use serde::{Deserialize, Serialize};
use thiserror::Error;

#[derive(Debug, Error)]
pub enum EmbeddingError {
    #[error("Failed to generate embedding: {0}")]
    GenerationError(String),
    #[error("Model not loaded: {0}")]
    ModelError(String),
}

#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct Embedding {
    pub vector: Array1<f32>,
    pub dimension: usize,
    pub model_name: String,
}

impl Embedding {
    pub fn new(vector: Array1<f32>, model_name: String) -> Self {
        Self {
            dimension: vector.len(),
            vector,
            model_name,
        }
    }

    pub fn cosine_similarity(&self, other: &Embedding) -> f32 {
        let dot_product = self.vector.dot(&other.vector);
        let norm1 = self.vector.dot(&self.vector).sqrt();
        let norm2 = other.vector.dot(&other.vector).sqrt();
        dot_product / (norm1 * norm2)
    }
}
