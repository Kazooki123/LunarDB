use crate::{Embedding, Model, EmbeddingError};
use serde::{Deserialize, Serialize};
use core::result::Result::Ok;
use std::collections::HashMap;
use thiserror::Error;
use tokio::sync::RwLock;
use std::sync::Arc;

#[derive(Debug, Error)]
pub enum AIError {
    #[error("Model not found: {0}")]
    ModelNotFound(String),
    #[error("Configuration error: {0}")]
    ConfigError(String),
    #[error("Pipeline error: {0}")]
    PipelineError(String),
    #[error("Embedding error: {0}")]
    EmbeddingError(#[from] EmbeddingError),
}

#[derive(Debug, Serialize, Deserialize)]
pub struct AIConfig {
    pub model_type: String,
    pub dimension: usize,
    pub context_window: usize,
    pub batch_size: usize,
    pub cache_size: usize,
}

impl Default for AIConfig {
    fn default() -> Self {
        Self {
            model_type: "all-MiniLM-L6-v2".to_string(),
            dimension: 384,
            context_window: 512,
            batch_size: 32,
            cache_size: 10000,
        }
    }
}

pub struct AIPipeline {
    config: AIConfig,
    models: Arc<RwLock<HashMap<String, Model>>>,
    embedding_cache: lru::LruCache<String, Embedding>,
}

impl AIPipeline {
    pub fn new(config: AIConfig) -> Self {
        Self {
            models: Arc::new(RwLock::new(HashMap::new())),
            embedding_cache: lru::LruCache::new(config.cache_size.try_into().unwrap()),
            config,
        }
    }

    pub async fn load_model(&self, model_name: &str) -> Result<(), AIError> {
        let mut models = self.models.write().await;
        if !models.contains_key(model_name) {
            let model = Model::new(model_name)
                .map_err(|e| AIError::ConfigError(e.to_string()))?;
            models.insert(model_name.to_string(), model);
        }
        Ok(())
    }

    pub async fn get_model(&self, model_name: &str) -> Result<Arc<Model>, AIError> {
        let models = self.models.read().await;
        models
            .get(model_name)
            .cloned()
            .map(Arc::new)
            .ok_or_else(|| AIError::ModelNotFound(model_name.to_string()))
    }

    pub async fn generate_embedding(
        &mut self,
        text: &str,
        model_name: Option<String>,
    ) -> Result<Embedding, AIError> {
        let cache_key = format!("{}:{}", model_name.as_deref().unwrap_or(&self.config.model_type), text);

        if let Some(cached) = self.embedding_cache.get(&cache_key) {
            return Ok(cached.clone());
        }

        let model_name = model_name.unwrap_or_else(|| self.config.model_type.clone());
        let model = self.get_model(&model_name).await?;

        let embedding = model.generate_embedding(text)
            .await
            .map_err(|e| AIError::PipelineError(e.to_string()))?;

        self.embedding_cache.put(cache_key, embedding.clone());
        Ok(embedding)
    }

    pub async fn batch_generate_embeddings(
        &mut self,
        texts: Vec<String>,
        model_name: Option<String>,
    ) -> Result<Vec<Embedding>, AIError> {
        use futures::stream::{StreamExt, iter};
        use futures::stream;

        let model_name = model_name.unwrap_or_else(|| self.config.model_type.clone());
        let chunks = texts.chunks(self.config.batch_size);

        let mut results = Vec::with_capacity(texts.len());

        for chunk in chunks {
            let futures = chunk.iter().map(|text| {
                self.generate_embedding(text, Some(model_name.clone()))
            });

            let chunk_results: Vec<_> = stream::iter(futures)
                .buffer_unordered(self.config.batch_size)
                .collect()
                .await;

            results.extend(chunk_results.into_iter().collect::<Result<Vec<_>, _>>()?);
        }

        Ok(results)
    }

    pub fn update_config(&mut self, new_config: AIConfig) {
        self.config = new_config;
        self.embedding_cache = lru::LruCache::new(self.config.cache_size.try_into().unwrap());
    }
}

pub trait ModelProvider {
    fn get_model_info(&self) -> HashMap<String, String>;
    fn supports_model(&self, model_name: &str) -> bool;
}

pub trait EmbeddingProvider {
    fn supports_dimension(&self, dimension: usize) -> bool;
    fn get_supported_dimensions(&self) -> Vec<usize>;
}

impl ModelProvider for AIPipeline {
    fn get_model_info(&self) -> HashMap<String, String> {
        let mut info = HashMap::new();
        info.insert(
            "default_model".to_string(),
            self.config.model_type.clone(),
        );
        info.insert(
            "dimension".to_string(),
            self.config.dimension.to_string(),
        );
        info
    }

    fn supports_model(&self, model_name: &str) -> bool {
        matches!(
            model_name,
            "all-MiniLM-L6-v2" | "all-mpnet-base-v2" | "all-distilroberta-v1"
        )
    }
}

impl EmbeddingProvider for AIPipeline {
    fn supports_dimension(&self, dimension: usize) -> bool {
        self.get_supported_dimensions().contains(&dimension)
    }

    fn get_supported_dimensions(&self) -> Vec<usize> {
        vec![384, 768, 1024]  // Common embedding dimensions
    }
}

// Tests
#[cfg(test)]
mod tests {
    use super::*;

    #[tokio::test]
    async fn test_pipeline_creation() {
        let config = AIConfig::default();
        let pipeline = AIPipeline::new(config);
        assert_eq!(pipeline.config.model_type, "all-MiniLM-L6-v2");
    }

    #[tokio::test]
    async fn test_model_loading() {
        let config = AIConfig::default();
        let pipeline = AIPipeline::new(config);
        let result = pipeline.load_model("all-MiniLM-L6-v2").await;
        assert!(result.is_ok());
    }
}
