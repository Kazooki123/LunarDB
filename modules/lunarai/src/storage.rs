use serde::{Deserialize, Serialize};
use std::collections::HashMap;
use std::fs::{self, File};
use std::io::{BufReader, BufWriter};
use std::path::{Path, PathBuf};
use crate::LunarAIError;

#[derive(Serialize, Deserialize)]
pub struct StorageMetadata {
    version: String,
    created_at: String,
    num_documents: usize,
    total_embeddings: usize,
}

pub struct Storage {
    base_path: PathBuf,
    metadata: StorageMetadata,
    embeddings_cache: HashMap<String, Vec<f32>>,
    max_cache_size: usize,
}

impl Storage {
    pub fn new<P: AsRef<Path>>(base_path: P) -> Result<Self, LunarAIError> {
        let base_path = base_path.as_ref().to_path_buf();
        fs::create_dir_all(&base_path).map_err(|e| {
            LunarAIError::StorageError(format!("Failed to create storage directory: {}", e))
        })?;

        let metadata = StorageMetadata {
            version: env!("CARGO_PKG_VERSION").to_string(),
            created_at: chrono::Utc::now().to_rfc3339(),
            num_documents: 0,
            total_embeddings: 0,
        };

        Ok(Self {
            base_path,
            metadata,
            embeddings_cache: HashMap::new(),
            max_cache_size: 1000, // Default cache size
        })
    }

    // Save embedding to disk
    pub fn save_embedding(&mut self, key: &str, embedding: Vec<f32>) -> Result<(), LunarAIError> {
        let embedding_path = self.get_embedding_path(key);
        let file = File::create(embedding_path).map_err(|e| {
            LunarAIError::StorageError(format!("Failed to create embedding file: {}", e))
        })?;

        let writer = BufWriter::new(file);
        serde_json::to_writer(writer, &embedding).map_err(|e| {
            LunarAIError::StorageError(format!("Failed to write embedding: {}", e))
        })?;

        // Update cache
        if self.embeddings_cache.len() >= self.max_cache_size {
            self.embeddings_cache.clear(); // Simple cache eviction strategy
        }
        self.embeddings_cache.insert(key.to_string(), embedding);

        // Update metadata
        self.metadata.total_embeddings += 1;
        self.save_metadata()?;

        Ok(())
    }

    // Load embedding from disk or cache
    pub fn load_embedding(&mut self, key: &str) -> Result<Vec<f32>, LunarAIError> {
        // Check cache first
        if let Some(embedding) = self.embeddings_cache.get(key) {
            return Ok(embedding.clone());
        }

        // Load from disk
        let embedding_path = self.get_embedding_path(key);
        let file = File::open(embedding_path).map_err(|e| {
            LunarAIError::StorageError(format!("Failed to open embedding file: {}", e))
        })?;

        let reader = BufReader::new(file);
        let embedding: Vec<f32> = serde_json::from_reader(reader).map_err(|e| {
            LunarAIError::StorageError(format!("Failed to read embedding: {}", e))
        })?;

        // Update cache
        if self.embeddings_cache.len() >= self.max_cache_size {
            self.embeddings_cache.clear();
        }
        self.embeddings_cache.insert(key.to_string(), embedding.clone());

        Ok(embedding)
    }

    // Delete embedding
    pub fn delete_embedding(&mut self, key: &str) -> Result<(), LunarAIError> {
        let embedding_path = self.get_embedding_path(key);
        if embedding_path.exists() {
            fs::remove_file(embedding_path).map_err(|e| {
                LunarAIError::StorageError(format!("Failed to delete embedding file: {}", e))
            })?;

            self.embeddings_cache.remove(key);
            self.metadata.total_embeddings = self.metadata.total_embeddings.saturating_sub(1);
            self.save_metadata()?;
        }
        Ok(())
    }

    // Save document metadata
    pub fn save_document_metadata<T: Serialize>(
        &mut self,
        doc_id: &str,
        metadata: &T,
    ) -> Result<(), LunarAIError> {
        let metadata_path = self.get_metadata_path(doc_id);
        let file = File::create(metadata_path).map_err(|e| {
            LunarAIError::StorageError(format!("Failed to create metadata file: {}", e))
        })?;

        let writer = BufWriter::new(file);
        serde_json::to_writer(writer, metadata).map_err(|e| {
            LunarAIError::StorageError(format!("Failed to write metadata: {}", e))
        })?;

        self.metadata.num_documents += 1;
        self.save_metadata()?;

        Ok(())
    }

    // Load document metadata
    pub fn load_document_metadata<T: for<'de> Deserialize<'de>>(
        &self,
        doc_id: &str,
    ) -> Result<T, LunarAIError> {
        let metadata_path = self.get_metadata_path(doc_id);
        let file = File::open(metadata_path).map_err(|e| {
            LunarAIError::StorageError(format!("Failed to open metadata file: {}", e))
        })?;

        let reader = BufReader::new(file);
        serde_json::from_reader(reader).map_err(|e| {
            LunarAIError::StorageError(format!("Failed to read metadata: {}", e))
        })
    }

    // Configure cache size
    pub fn set_cache_size(&mut self, size: usize) {
        self.max_cache_size = size;
        if self.embeddings_cache.len() > size {
            self.embeddings_cache.clear();
        }
    }

    // Clear cache
    pub fn clear_cache(&mut self) {
        self.embeddings_cache.clear();
    }

    // Get storage statistics
    pub fn get_stats(&self) -> Result<StorageStats, LunarAIError> {
        let total_size = self.calculate_storage_size()?;

        Ok(StorageStats {
            num_documents: self.metadata.num_documents,
            total_embeddings: self.metadata.total_embeddings,
            cache_size: self.embeddings_cache.len(),
            max_cache_size: self.max_cache_size,
            storage_size_bytes: total_size,
        })
    }

    // Helper methods
    fn get_embedding_path(&self, key: &str) -> PathBuf {
        self.base_path.join("embeddings").join(format!("{}.json", key))
    }

    fn get_metadata_path(&self, doc_id: &str) -> PathBuf {
        self.base_path.join("metadata").join(format!("{}.json", doc_id))
    }

    fn save_metadata(&self) -> Result<(), LunarAIError> {
        let metadata_file = self.base_path.join("storage_metadata.json");
        let file = File::create(metadata_file).map_err(|e| {
            LunarAIError::StorageError(format!("Failed to create metadata file: {}", e))
        })?;

        let writer = BufWriter::new(file);
        serde_json::to_writer_pretty(writer, &self.metadata).map_err(|e| {
            LunarAIError::StorageError(format!("Failed to write metadata: {}", e))
        })
    }

    fn calculate_storage_size(&self) -> Result<u64, LunarAIError> {
        let mut total_size = 0;
        for entry in fs::read_dir(&self.base_path).map_err(|e| {
            LunarAIError::StorageError(format!("Failed to read directory: {}", e))
        })? {
            let entry = entry.map_err(|e| {
                LunarAIError::StorageError(format!("Failed to read directory entry: {}", e))
            })?;

            if entry.path().is_file() {
                total_size += entry.metadata().map_err(|e| {
                    LunarAIError::StorageError(format!("Failed to read file metadata: {}", e))
                })?.len();
            }
        }
        Ok(total_size)
    }
}

#[derive(Debug, Serialize)]
pub struct StorageStats {
    num_documents: usize,
    total_embeddings: usize,
    cache_size: usize,
    max_cache_size: usize,
    storage_size_bytes: u64,
}

#[cfg(test)]
mod tests {
    use super::*;
    use tempfile::tempdir;

    #[test]
    fn test_storage_operations() -> Result<(), LunarAIError> {
        let temp_dir = tempdir().unwrap();
        let mut storage = Storage::new(temp_dir.path())?;

        // Test saving and loading embedding
        let test_embedding = vec![1.0, 2.0, 3.0];
        storage.save_embedding("test1", test_embedding.clone())?;
        let loaded = storage.load_embedding("test1")?;
        assert_eq!(test_embedding, loaded);

        // Test metadata operations
        #[derive(Serialize, Deserialize, PartialEq, Debug)]
        struct TestMetadata {
            value: String,
        }

        let metadata = TestMetadata {
            value: "test".to_string(),
        };
        storage.save_document_metadata("doc1", &metadata)?;
        let loaded_metadata: TestMetadata = storage.load_document_metadata("doc1")?;
        assert_eq!(metadata.value, loaded_metadata.value);

        // Test stats
        let stats = storage.get_stats()?;
        assert!(stats.num_documents > 0);
        assert!(stats.total_embeddings > 0);

        Ok(())
    }
}
