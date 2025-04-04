use std::hash::{Hash, Hasher};
use std::collections::hash_map::DefaultHasher;
use crate::core::Result;

const SEED_PRIME: u64 = 0x517cc1b727220a95;

pub struct VectorHasher {
    precision: i32,
    seed: u64,
}

impl Default for VectorHasher {
    fn default() -> Self {
        Self {
            precision: 6,
            seed: SEED_PRIME,
        }
    }
}

impl VectorHasher {
    pub fn new(precision: i32, seed: u64) -> Self {
        Self { precision, seed }
    }

    pub fn hash_vector(&self, vector: &[f32]) -> u64 {
        let mut hasher = DefaultHasher::new();
        hasher.write_u64(self.seed);

        for &value in vector {
            let discrete_value = (value * 10f32.powi(self.precision)) as i64;
            discrete_value.hash(&mut hasher);
        }

        hasher.finish()
    }

    pub fn hash_vectors(&self, vectors: &[Vec<f32>]) -> Vec<u64> {
        vectors.iter()
            .map(|v| self.hash_vector(v))
            .collect()
    }
}

pub struct SimHash {
    num_bits: usize,
    random_projections: Vec<Vec<f32>>,
}

impl SimHash {
    pub fn new(dimension: usize, num_bits: usize) -> Self {
        use rand::Rng;
        let mut rng = rand::thread_rng();

        let random_projections: Vec<Vec<f32>> = (0..num_bits)
            .map(|_| {
                (0..dimension)
                    .map(|_| rng.gen_range(-1.0..1.0))
                    .collect()
            })
            .collect();

        Self {
            num_bits,
            random_projections,
        }
    }

    pub fn hash(&self, vector: &[f32]) -> Result<u64> {
        if vector.len() != self.random_projections[0].len() {
            return Err(crate::core::VectorError::DimensionMismatch);
        }

        let mut hash_value = 0u64;

        for (i, projection) in self.random_projections.iter().enumerate() {
            let dot_product: f32 = vector.iter()
                .zip(projection.iter())
                .map(|(a, b)| a * b)
                .sum();

            if dot_product > 0.0 {
                hash_value |= 1 << i;
            }
        }

        Ok(hash_value)
    }

    pub fn hamming_distance(a: u64, b: u64) -> u32 {
        (a ^ b).count_ones()
    }
}

#[derive(Default)]
pub struct MultiHasher {
    hashers: Vec<VectorHasher>,
}

impl MultiHasher {
    pub fn new(num_hashers: usize, base_precision: i32) -> Self {
        let hashers: Vec<VectorHasher> = (0..num_hashers)
            .map(|i| {
                let seed = SEED_PRIME.wrapping_add(i as u64);
                VectorHasher::new(base_precision, seed)
            })
            .collect();

        Self { hashers }
    }

    pub fn hash_vector(&self, vector: &[f32]) -> Vec<u64> {
        self.hashers.iter()
            .map(|hasher| hasher.hash_vector(vector))
            .collect()
    }
}
