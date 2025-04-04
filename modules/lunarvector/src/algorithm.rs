use crate::core::{DistanceMetric, Result, VectorError};

pub struct CosineDistance;
pub struct EuclideanDistance;
pub struct ManhattanDistance;
pub struct DotProductSimilarity;

impl DistanceMetric for CosineDistance {
    fn distance(&self, a: &[f32], b: &[f32]) -> Result<f32> {
        if a.len() != b.len() {
            return Err(VectorError::DimensionMismatch);
        }

        let dot_product: f32 = a.iter().zip(b.iter()).map(|(x, y)| x * y).sum();
        let norm_a: f32 = a.iter().map(|x| x * x).sum::<f32>().sqrt();
        let norm_b: f32 = b.iter().map(|x| x * x).sum::<f32>().sqrt();

        if norm_a == 0.0 || norm_b == 0.0 {
            return Err(VectorError::InvalidOperation);
        }

        Ok(1.0 - (dot_product / (norm_a * norm_b)))
    }
}

impl DistanceMetric for EuclideanDistance {
    fn distance(&self, a: &[f32], b: &[f32]) -> Result<f32> {
        if a.len() != b.len() {
            return Err(VectorError::DimensionMismatch);
        }

        let sum: f32 = a.iter()
            .zip(b.iter())
            .map(|(x, y)| (x - y) * (x - y))
            .sum();

        Ok(sum.sqrt())
    }
}

impl DistanceMetric for ManhattanDistance {
    fn distance(&self, a: &[f32], b: &[f32]) -> Result<f32> {
        if a.len() != b.len() {
            return Err(VectorError::DimensionMismatch);
        }

        let distance: f32 = a.iter()
            .zip(b.iter())
            .map(|(x, y)| (x - y).abs())
            .sum();

        Ok(distance)
    }
}

impl DistanceMetric for DotProductSimilarity {
    fn distance(&self, a: &[f32], b: &[f32]) -> Result<f32> {
        if a.len() != b.len() {
            return Err(VectorError::DimensionMismatch);
        }

        let similarity: f32 = a.iter()
            .zip(b.iter())
            .map(|(x, y)| x * y)
            .sum();

        Ok(-similarity) // Negative because smaller distances should indicate higher similarity
    }
}

pub fn normalize_vector(vector: &mut [f32]) -> Result<()> {
    let norm: f32 = vector.iter().map(|x| x * x).sum::<f32>().sqrt();
    
    if norm == 0.0 {
        return Err(VectorError::InvalidOperation);
    }

    for x in vector.iter_mut() {
        *x /= norm;
    }

    Ok(())
}

pub fn dimension_reduction(vector: &[f32], target_dim: usize) -> Result<Vec<f32>> {
    if target_dim >= vector.len() {
        return Err(VectorError::InvalidOperation);
    }

    // Simple average pooling for dimension reduction
    let segment_size = vector.len() / target_dim;
    let mut result = Vec::with_capacity(target_dim);

    for i in 0..target_dim {
        let start = i * segment_size;
        let end = if i == target_dim - 1 {
            vector.len()
        } else {
            (i + 1) * segment_size
        };

        let avg = vector[start..end].iter().sum::<f32>() / (end - start) as f32;
        result.push(avg);
    }

    Ok(result)
}
