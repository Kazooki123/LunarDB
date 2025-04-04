use core::{iter::Iterator, option::Option::{None, Some}};
use std::time::{Duration, Instant};

pub struct Vector {
    id: String,
    data: Vec<f32>,
    expiration: Option<Instant>,
}

impl Vector {
    pub fn new(id: String, data: Vec<f32>) -> Self {
        Vector {
            id,
            data,
            expiration: None,
        }
    }

    pub fn id(&self) -> &str {
        &self.id
    }

    pub fn data(&self) -> &Vec<f32> {
        &self.data
    }

    pub fn set_ttl(&mut self, ttl: Duration) {
        self.expiration = Some(Instant::now() + ttl);
    }

    pub fn is_expired(&self) -> bool {
        self.expiration.map_or(false, |exp| Instant::now() > exp)
    }
}

pub struct LunarVector {
    vectors: Vec<Vector>,
}

impl LunarVector {
    pub fn new() -> Self {
        LunarVector {
            vectors: Vec::new(),
        }
    }

    pub fn add(&mut self, id: String, data: Vec<f32>) {
        self.vectors.push(Vector::new(id, data));
    }

    pub fn get(&self, id: &str) -> Option<&Vector> {
        self.vectors.iter().find(|v| v.id() == id)
    }

    pub fn len(&self) -> usize {
        self.vectors.len()
    }

    pub fn is_empty(&self) -> bool {
        self.vectors.is_empty()
    }

    pub fn set_ttl(&mut self, id: &str, ttl: Duration) -> bool {
        if let Some(vector) = self.vectors.iter_mut().find(|v| v.id() == id) {
            vector.set_ttl(ttl);
            true
        } else {
            false
        }
    }

    pub fn cleanup_expired(&mut self) {
        self.vectors.retain(|v| !v.is_expired());
    }

    pub fn search_similar(&self, query: &[f32], limit: usize) -> Vec<(String, f32)> {
        let mut similarities: Vec<(String, f32)> = self.vectors
            .iter()
            .map(|v| (v.id().to_string(), cosine_similarity(query, v.data())))
            .collect();

        similarities.sort_by(|a, b| b.1.partial_cmp(&a.1).unwrap());
        similarities.truncate(limit);
        similarities
    }
}

fn cosine_similarity(a: &[f32], b: &[f32]) -> f32 {
    let dot_product: f32 = a.iter().zip(b.iter()).map(|(x, y)| x * y).sum();
    let magnitude_a: f32 = a.iter().map(|x| x * x).sum::<f32>().sqrt();
    let magnitude_b: f32 = b.iter().map(|x| x * x).sum::<f32>().sqrt();
    dot_product / (magnitude_a * magnitude_b)
}
