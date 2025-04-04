use std::collections::{HashMap, BinaryHeap};
use std::cmp::Ordering;
use crate::core::{DistanceMetric, Result, VectorError, VectorIndex};
use crate::algorithm::CosineDistance;
use crate::hash::{SimHash, MultiHasher};

struct Neighbor {
    id: String,
    distance: f32,
}

impl Ord for Neighbor {
    fn cmp(&self, other: &Self) -> Ordering {
        other.distance.partial_cmp(&self.distance).unwrap_or(Ordering::Equal)
    }
}

impl PartialOrd for Neighbor {
    fn partial_cmp(&self, other: &Self) -> Option<Ordering> {
        Some(self.cmp(other))
    }
}

impl Eq for Neighbor {}

impl PartialEq for Neighbor {
    fn eq(&self, other: &Self) -> bool {
        self.distance == other.distance
    }
}

pub struct FlatIndex {
    vectors: HashMap<String, Vec<f32>>,
    distance: Box<dyn DistanceMetric>,
}

impl FlatIndex {
    pub fn new(distance: Box<dyn DistanceMetric>) -> Self {
        Self {
            vectors: HashMap::new(),
            distance,
        }
    }

    pub fn with_cosine() -> Self {
        Self::new(Box::new(CosineDistance))
    }
}

impl VectorIndex for FlatIndex {
    fn insert(&mut self, id: String, vector: Vec<f32>) -> Result<()> {
        self.vectors.insert(id, vector);
        Ok(())
    }

    fn search(&self, query: &[f32], k: usize) -> Result<Vec<(String, f32)>> {
        let mut heap = BinaryHeap::new();

        for (id, vector) in &self.vectors {
            let distance = self.distance.distance(query, vector)?;
            heap.push(Neighbor {
                id: id.clone(),
                distance,
            });
            if heap.len() > k {
                heap.pop();
            }
        }

        Ok(heap.into_sorted_vec()
            .into_iter()
            .map(|n| (n.id, n.distance))
            .collect())
    }

    fn remove(&mut self, id: &str) -> Result<()> {
        self.vectors.remove(id);
        Ok(())
    }

    fn size(&self) -> usize {
        self.vectors.len()
    }
}

pub struct LSHIndex {
    hasher: SimHash,
    hash_tables: Vec<HashMap<u64, Vec<String>>>,
    vectors: HashMap<String, Vec<f32>>,
    distance: Box<dyn DistanceMetric>,
}

impl LSHIndex {
    pub fn new(
        dimension: usize,
        num_hash_tables: usize,
        num_bits: usize,
        distance: Box<dyn DistanceMetric>,
    ) -> Self {
        Self {
            hasher: SimHash::new(dimension, num_bits),
            hash_tables: vec![HashMap::new(); num_hash_tables],
            vectors: HashMap::new(),
            distance,
        }
    }
}

impl VectorIndex for LSHIndex {
    fn insert(&mut self, id: String, vector: Vec<f32>) -> Result<()> {
        let hash = self.hasher.hash(&vector)?;
        
        for table in &mut self.hash_tables {
            table.entry(hash)
                .or_insert_with(Vec::new)
                .push(id.clone());
        }
        
        self.vectors.insert(id, vector);
        Ok(())
    }

    fn search(&self, query: &[f32], k: usize) -> Result<Vec<(String, f32)>> {
        let query_hash = self.hasher.hash(query)?;
        let mut candidates = HashMap::new();

        for table in &self.hash_tables {
            if let Some(bucket) = table.get(&query_hash) {
                for id in bucket {
                    candidates.insert(id.clone(), ());
                }
            }
        }

        let mut results = Vec::new();
        for id in candidates.keys() {
            if let Some(vector) = self.vectors.get(id) {
                let distance = self.distance.distance(query, vector)?;
                results.push((id.clone(), distance));
            }
        }

        results.sort_by(|a, b| a.1.partial_cmp(&b.1).unwrap_or(Ordering::Equal));
        results.truncate(k);
        
        Ok(results)
    }

    fn remove(&mut self, id: &str) -> Result<()> {
        if let Some(vector) = self.vectors.remove(id) {
            let hash = self.hasher.hash(&vector)?;
            
            for table in &mut self.hash_tables {
                if let Some(bucket) = table.get_mut(&hash) {
                    bucket.retain(|x| x != id);
                }
            }
        }
        Ok(())
    }

    fn size(&self) -> usize {
