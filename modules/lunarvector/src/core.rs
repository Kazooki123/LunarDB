use std::error::Error;
use std::fmt;
use std::time::Duration;

#[derive(Debug)]
pub enum VectorError {
    DimensionMismatch,
    InvalidOperation,
    StorageError(String),
    IndexError(String),
}

impl Error for VectorError {}

impl fmt::Display for VectorError {
    fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
        match self {
            VectorError::DimensionMismatch => write!(f, "Vector dimensions do not match"),
            VectorError::InvalidOperation => write!(f, "Invalid vector operation"),
            VectorError::StorageError(msg) => write!(f, "Storage error: {}", msg),
            VectorError::IndexError(msg) => write!(f, "Index error: {}", msg),
        }
    }
}

pub type Result<T> = std::result::Result<T, VectorError>;

pub trait VectorIndex {
    fn insert(&mut self, id: String, vector: Vec<f32>) -> Result<()>;
    fn search(&self, query: &[f32], k: usize) -> Result<Vec<(String, f32)>>;
    fn remove(&mut self, id: &str) -> Result<()>;
    fn size(&self) -> usize;
}

pub trait DistanceMetric {
    fn distance(&self, a: &[f32], b: &[f32]) -> Result<f32>;
}

pub trait VectorStore {
    fn add(&mut self, id: String, vector: Vec<f32>, ttl: Option<Duration>) -> Result<()>;
    fn get(&self, id: &str) -> Result<Option<&Vec<f32>>>;
    fn delete(&mut self, id: &str) -> Result<bool>;
    fn cleanup_expired(&mut self);
}

#[derive(Debug, Clone, PartialEq)]
pub struct VectorMeta {
    pub id: String,
    pub dimension: usize,
    pub created_at: std::time::SystemTime,
    pub ttl: Option<Duration>,
}

pub trait VectorMetadata {
    fn meta(&self) -> &VectorMeta;
    fn is_expired(&self) -> bool;
}
