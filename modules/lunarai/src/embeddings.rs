use tch::Tensor;
use serde::{Deserialize, Serialize};
use thiserror::Error;
use core::result::Result::Ok;
use tch::TchError;

#[derive(Debug, Error)]
pub enum EmbeddingError {
    #[error("Failed to generate embedding: {0}")]
    GenerationError(String),
    #[error("Model not loaded: {0}")]
    ModelError(String),
    #[error("Device error: {0}")]
    DeviceError(String),
}

#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct Embedding {
    pub vector: Vec<f32>,
    pub dimension: usize,
    pub model_name: String,
}

impl Embedding {
    pub fn from_tensor(tensor: Tensor, model_name: String) -> Result<Self, EmbeddingError> {
        let vector: Vec<f32> = Vec::<f32>::try_from(tensor.flatten(0, -1))
            .map_err(|e: TchError| EmbeddingError::DeviceError(e.to_string()))?;

        Ok(Self {
            dimension: vector.len(),
            vector,
            model_name,
        })
    }

    pub fn cosine_similarity(&self, other: &Embedding) -> f32 {
        let dot_product: f32 = self.vector.iter()
            .zip(other.vector.iter())
            .map(|(a, b)| a * b)
            .sum();

        let norm1: f32 = self.vector.iter()
            .map(|x| x * x)
            .sum::<f32>()
            .sqrt();

        let norm2: f32 = other.vector.iter()
            .map(|x| x * x)
            .sum::<f32>()
            .sqrt();

        dot_product / (norm1 * norm2)
    }
}
