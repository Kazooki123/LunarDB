use std::collections::HashMap;
use std::sync::{Arc, RwLock};
use std::time::{Duration, Instant};
use crate::core::{Result, VectorError, VectorStore};

struct VectorEntry {
    vector: Vec<f32>,
    created_at: Instant,
    ttl: Option<Duration>,
}

impl VectorEntry {
    fn new(vector: Vec<f32>, ttl: Option<Duration>) -> Self {
        Self {
            vector,
            created_at: Instant::now(),
            ttl,
        }
    }

    fn is_expired(&self) -> bool {
        if let Some(ttl) = self.ttl {
            self.created_at.elapsed() >= ttl
        } else {
            false
        }
    }
}

pub struct MemoryStore {
    vectors: HashMap<String, VectorEntry>,
}

impl Default for MemoryStore {
    fn default() -> Self {
        Self {
            vectors: HashMap::new(),
        }
    }
}

impl VectorStore for MemoryStore {
    fn add(&mut self, id: String, vector: Vec<f32>, ttl: Option<Duration>) -> Result<()> {
        let entry = VectorEntry::new(vector, ttl);
        self.vectors.insert(id, entry);
        Ok(())
    }

    fn get(&self, id: &str) -> Result<Option<&Vec<f32>>> {
        Ok(self.vectors.get(id).and_then(|entry| {
            if entry.is_expired() {
                None
            } else {
                Some(&entry.vector)
            }
        }))
    }

    fn delete(&mut self, id: &str) -> Result<bool> {
        Ok(self.vectors.remove(id).is_some())
    }

    fn cleanup_expired(&mut self) {
        self.vectors.retain(|_, entry| !entry.is_expired());
    }
}

pub struct ConcurrentStore {
    vectors: Arc<RwLock<HashMap<String, VectorEntry>>>,
}

impl Default for ConcurrentStore {
    fn default() -> Self {
        Self {
            vectors: Arc::new(RwLock::new(HashMap::new())),
        }
    }
}

impl ConcurrentStore {
    pub fn new() -> Self {
        Self::default()
    }

    pub fn clone_store(&self) -> Self {
        Self {
            vectors: Arc::clone(&self.vectors),
        }
    }
}

impl VectorStore for ConcurrentStore {
    fn add(&mut self, id: String, vector: Vec<f32>, ttl: Option<Duration>) -> Result<()> {
        let entry = VectorEntry::new(vector, ttl);
        self.vectors
            .write()
            .map_err(|_| VectorError::StorageError("Lock poisoned".to_string()))?
            .insert(id, entry);
        Ok(())
    }

    fn get(&self, id: &str) -> Result<Option<&Vec<f32>>> {
        // Note: This implementation holds the read lock until the reference is dropped
        let guard = self.vectors
            .read()
            .map_err(|_| VectorError::StorageError("Lock poisoned".to_string()))?;
            
        if let Some(entry) = guard.get(id) {
            if !entry.is_expired() {
                return Ok(Some(&entry.vector));
            }
        }
        Ok(None)
    }

    fn delete(&mut self, id: &str) -> Result<bool> {
        Ok(self.vectors
            .write()
            .map_err(|_| VectorError::StorageError("Lock poisoned".to_string()))?
            .remove(id)
            .is_some())
    }

    fn cleanup_expired(&mut self) {
        if let Ok(mut guard) = self.vectors.write() {
            guard.retain(|_, entry| !entry.is_expired());
        }
    }
}
