use crate::{Embedding, EmbeddingError, ModelManager};
use std::num::NonZero;
use serde::{Deserialize, Serialize};
use tokio::sync::RwLock;
use std::sync::Arc;
use std::path::PathBuf;
use lru::LruCache;

#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct AIConfig {
    pub model_path: PathBuf,
    pub cache_size: usize,
    pub batch_size: usize,
    pub use_gpu: bool,
}

impl Default for AIConfig {
    fn default() -> Self {
        Self {
            model_path: PathBuf::from(r"D:\huggingface_cache\models--gpt2\snapshots\607a30d783dfa663caf39e06633721c8d4cfcd7e"),
            cache_size: 10000,
            batch_size: 32,
            use_gpu: true,
        }
    }
}

#[derive(Debug, Clone)]
pub struct AIPipeline {
    config: AIConfig,
    model_manager: Arc<RwLock<ModelManager>>,
    embedding_cache: Arc<RwLock<LruCache<String, Embedding>>>,
}

impl AIPipeline {
    pub fn new(config: AIConfig) -> Self {
        Self {
            model_manager: Arc::new(RwLock::new(ModelManager::new(config.model_path.clone()))),
            embedding_cache: Arc::new(RwLock::new(LruCache::new(NonZero::new(config.cache_size).unwrap()))),
            config,
        }
    }

    pub async fn initialize(&self) -> Result<(), EmbeddingError> {
        let mut manager = self.model_manager.write().await;
        manager.load_gpt2()
            .map_err(|e| EmbeddingError::ModelError(e.to_string()))
    }

    pub async fn generate_embedding(&self, text: &str) -> Result<Embedding, EmbeddingError> {
        let cache_key = text.to_string();

        // Check cache first
        {
            let mut cache = self.embedding_cache.write().await;
            if let Some(cached) = cache.get(&cache_key) {
                return Ok(cached.clone());
            }
        }

        // Generate new embedding
        let manager = self.model_manager.read().await;
        let tensor = manager.get_embeddings(text)
            .map_err(|e| EmbeddingError::GenerationError(e.to_string()))?;

        let embedding = Embedding::from_tensor(tensor, "gpt2".to_string())?;

        // Update cache
        {
            let mut cache = self.embedding_cache.write().await;
            cache.put(cache_key, embedding.clone());
        }

        Ok(embedding)
    }
}
