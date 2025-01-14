use nalgebra as na;
use rand_distr::{Normal, Distribution};
use std::collections::HashMap;
use crate::LunarAIError;

pub struct EmbeddingEngine {
    dimension: usize,
    embeddings: HashMap<String, Vec<f32>>,
    random_projection: Option<na::DMatrix<f32>>,
}

impl EmbeddingEngine {
    pub fn new() -> Self {
        Self {
            dimension: 384, // Default dimension similar to small language models
            embeddings: HashMap::new(),
            random_projection: None,
        }
    }

    pub fn with_dimension(dimension: usize) -> Self {
        Self {
            dimension,
            embeddings: HashMap::new(),
            random_projection: None,
        }
    }

    // Initialize random projection matrix for dimensionality reduction
    pub fn init_random_projection(&mut self, target_dim: usize) -> Result<(), LunarAIError> {
        if target_dim >= self.dimension {
            return Err(LunarAIError::InvalidDimensions(
                "Target dimension must be smaller than current dimension".to_string(),
            ));
        }

        let normal = Normal::new(0.0, 1.0 / (target_dim as f32).sqrt())
            .map_err(|e| LunarAIError::EmbeddingError(e.to_string()))?;

        let mut rng = rand::thread_rng();
        let matrix = na::DMatrix::from_fn(target_dim, self.dimension, |_, _| {
            normal.sample(&mut rng)
        });

        self.random_projection = Some(matrix);
        Ok(())
    }

    // Generate embeddings using random projections and positional encoding
    pub fn generate_embedding(&self, text: &str) -> Result<Vec<f32>, LunarAIError> {
        let tokens = text.split_whitespace().collect::<Vec<_>>();
        let mut embedding = vec![0.0; self.dimension];

        for (pos, token) in tokens.iter().enumerate() {
            let token_hash = self.hash_token(token);
            let pos_encoding = self.positional_encoding(pos, self.dimension);

            for i in 0..self.dimension {
                embedding[i] += (token_hash + pos_encoding[i]).sin();
            }
        }

        // Normalize the embedding
        let norm: f32 = embedding.iter().map(|x| x * x).sum::<f32>().sqrt();
        if norm > 0.0 {
            embedding.iter_mut().for_each(|x| *x /= norm);
        }

        Ok(embedding)
    }

    // Store embedding for a text
    pub fn store_embedding(&mut self, key: String, embedding: Vec<f32>) -> Result<(), LunarAIError> {
        if embedding.len() != self.dimension {
            return Err(LunarAIError::InvalidDimensions(
                "Embedding dimension mismatch".to_string(),
            ));
        }
        self.embeddings.insert(key, embedding);
        Ok(())
    }

    // Compute similarity between two embeddings using cosine similarity
    pub fn similarity(&self, emb1: &[f32], emb2: &[f32]) -> Result<f32, LunarAIError> {
        if emb1.len() != emb2.len() {
            return Err(LunarAIError::InvalidDimensions(
                "Embeddings must have same dimension".to_string(),
            ));
        }

        let dot_product: f32 = emb1.iter().zip(emb2).map(|(a, b)| a * b).sum();
        let norm1: f32 = emb1.iter().map(|x| x * x).sum::<f32>().sqrt();
        let norm2: f32 = emb2.iter().map(|x| x * x).sum::<f32>().sqrt();

        Ok(dot_product / (norm1 * norm2))
    }

    // Helper function to create positional encoding
    fn positional_encoding(&self, position: usize, dim: usize) -> Vec<f32> {
        let mut encoding = vec![0.0; dim];
        for i in 0..dim {
            let frequency = 1.0 / (10000.0_f32.powf(2.0 * (i / 2) as f32 / dim as f32));
            encoding[i] = if i % 2 == 0 {
                (position as f32 * frequency).sin()
            } else {
                (position as f32 * frequency).cos()
            };
        }
        encoding
    }

    // Helper function to hash tokens
    fn hash_token(&self, token: &str) -> f32 {
        let mut hash = 0u64;
        for byte in token.bytes() {
            hash = hash.wrapping_mul(31).wrapping_add(byte as u64);
        }
        (hash as f32) / (u64::MAX as f32)
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_embedding_generation() {
        let engine = EmbeddingEngine::new();
        let embedding = engine.generate_embedding("test text").unwrap();
        assert_eq!(embedding.len(), 384);
    }

    #[test]
    fn test_embedding_similarity() {
        let engine = EmbeddingEngine::new();
        let emb1 = engine.generate_embedding("hello world").unwrap();
        let emb2 = engine.generate_embedding("hello world").unwrap();
        let similarity = engine.similarity(&emb1, &emb2).unwrap();
        assert!((similarity - 1.0).abs() < 1e-6);
    }
}
