use nalgebra as na;
use rand::Rng;
use rand_distr::{Normal, Distribution};
use crate::LunarAIError;

pub struct Dense {
    weights: na::DMatrix<f32>,
    biases: na::DVector<f32>,
    activation: ActivationFunction,
}

#[derive(Clone, Copy)]
pub enum ActivationFunction {
    ReLU,
    Sigmoid,
    Tanh,
}

impl Dense {
    pub fn new(input_dim: usize, output_dim: usize, activation: ActivationFunction) -> Result<Self, LunarAIError> {
        let normal = Normal::new(0.0, (2.0 / input_dim as f32).sqrt())
            .map_err(|e| LunarAIError::InvalidDimensions(e.to_string()))?;

        let mut rng = rand::thread_rng();
        let weights = na::DMatrix::from_fn(output_dim, input_dim, |_, _| {
            normal.sample(&mut rng)
        });

        let biases = na::DVector::zeros(output_dim);

        Ok(Self {
            weights,
            biases,
            activation,
        })
    }

    pub fn forward(&self, input: &na::DVector<f32>) -> Result<na::DVector<f32>, LunarAIError> {
        if input.nrows() != self.weights.ncols() {
            return Err(LunarAIError::InvalidDimensions(
                "Input dimension mismatch".to_string(),
            ));
        }

        let output = &self.weights * input + &self.biases;
        Ok(self.apply_activation(&output))
    }

    fn apply_activation(&self, x: &na::DVector<f32>) -> na::DVector<f32> {
        match self.activation {
            ActivationFunction::ReLU => x.map(|v| v.max(0.0)),
            ActivationFunction::Sigmoid => x.map(|v| 1.0 / (1.0 + (-v).exp())),
            ActivationFunction::Tanh => x.map(|v| v.tanh()),
        }
    }
}

// Simple dimensionality reduction using PCA-like approach
pub struct DimensionalityReducer {
    projection_matrix: Option<na::DMatrix<f32>>,
    target_dim: usize,
}

impl DimensionalityReducer {
    pub fn new(target_dim: usize) -> Self {
        Self {
            projection_matrix: None,
            target_dim,
        }
    }

    pub fn fit(&mut self, data: &[Vec<f32>]) -> Result<(), LunarAIError> {
        if data.is_empty() {
            return Err(LunarAIError::InvalidDimensions("Empty input data".to_string()));
        }

        let input_dim = data[0].len();
        if self.target_dim >= input_dim {
            return Err(LunarAIError::InvalidDimensions(
                "Target dimension must be smaller than input dimension".to_string(),
            ));
        }

        let normal = Normal::new(0.0, 1.0 / (self.target_dim as f32).sqrt())
            .map_err(|e| LunarAIError::InvalidDimensions(e.to_string()))?;

        let mut rng = rand::thread_rng();
        let matrix = na::DMatrix::from_fn(self.target_dim, input_dim, |_, _| {
            normal.sample(&mut rng)
        });

        self.projection_matrix = Some(matrix);
        Ok(())
    }

    pub fn transform(&self, input: &[f32]) -> Result<Vec<f32>, LunarAIError> {
        let projection_matrix = self.projection_matrix.as_ref().ok_or_else(|| {
            LunarAIError::ModelNotFound("Projection matrix not initialized".to_string())
        })?;

        if input.len() != projection_matrix.ncols() {
            return Err(LunarAIError::InvalidDimensions(
                "Input dimension mismatch".to_string(),
            ));
        }

        let input_vector = na::DVector::from_vec(input.to_vec());
        let result = projection_matrix * input_vector;
        Ok(result.as_slice().to_vec())
    }
}

// Clustering using k-means
pub struct KMeans {
    k: usize,
    centroids: Option<Vec<Vec<f32>>>,
    max_iterations: usize,
}

impl KMeans {
    pub fn new(k: usize, max_iterations: usize) -> Self {
        Self {
            k,
            centroids: None,
            max_iterations,
        }
    }

    pub fn fit(&mut self, data: &[Vec<f32>]) -> Result<(), LunarAIError> {
        if data.is_empty() {
            return Err(LunarAIError::InvalidDimensions("Empty input data".to_string()));
        }

        let mut rng = rand::thread_rng();
        let mut centroids: Vec<Vec<f32>> = (0..self.k)
            .map(|_| data[rng.gen_range(0..data.len())].clone())
            .collect();

        for _ in 0..self.max_iterations {
            let mut clusters: Vec<Vec<usize>> = vec![Vec::new(); self.k];

            // Assign points to nearest centroid
            for (idx, point) in data.iter().enumerate() {
                let closest = self.find_closest_centroid(point, &centroids)?;
                clusters[closest].push(idx);
            }

            // Update centroids
            let mut new_centroids = vec![vec![0.0; data[0].len()]; self.k];
            for (i, cluster) in clusters.iter().enumerate() {
                if cluster.is_empty() {
                    continue;
                }

                for &idx in cluster {
                    for j in 0..data[0].len() {
                        new_centroids[i][j] += data[idx][j];
                    }
                }

                for j in 0..data[0].len() {
                    new_centroids[i][j] /= cluster.len() as f32;
                }
            }

            if Self::has_converged(&centroids, &new_centroids) {
                break;
            }
            centroids = new_centroids;
        }

        self.centroids = Some(centroids);
        Ok(())
    }

    pub fn predict(&self, point: &[f32]) -> Result<usize, LunarAIError> {
        let centroids = self.centroids.as_ref().ok_or_else(|| {
            LunarAIError::ModelNotFound("Model not fitted".to_string())
        })?;

        self.find_closest_centroid(point, centroids)
    }

    fn find_closest_centroid(&self, point: &[f32], centroids: &[Vec<f32>]) -> Result<usize, LunarAIError> {
        let mut min_dist = f32::INFINITY;
        let mut closest = 0;

        for (i, centroid) in centroids.iter().enumerate() {
            if point.len() != centroid.len() {
                return Err(LunarAIError::InvalidDimensions(
                    "Point dimension mismatch".to_string(),
                ));
            }

            let dist: f32 = point.iter()
                .zip(centroid.iter())
                .map(|(a, b)| (a - b).powi(2))
                .sum();

            if dist < min_dist {
                min_dist = dist;
                closest = i;
            }
        }

        Ok(closest)
    }

    fn has_converged(old_centroids: &[Vec<f32>], new_centroids: &[Vec<f32>]) -> bool {
        let threshold = 1e-4;
        old_centroids.iter().zip(new_centroids.iter()).all(|(old, new)| {
            old.iter()
                .zip(new.iter())
                .all(|(a, b)| (a - b).abs() < threshold)
        })
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_dense_layer() {
        let layer = Dense::new(4, 2, ActivationFunction::ReLU).unwrap();
        let input = na::DVector::from_vec(vec![1.0, 2.0, 3.0, 4.0]);
        let output = layer.forward(&input).unwrap();
        assert_eq!(output.nrows(), 2);
    }

    #[test]
    fn test_dimensionality_reducer() {
        let mut reducer = DimensionalityReducer::new(2);
        let data = vec![vec![1.0, 2.0, 3.0, 4.0], vec![5.0, 6.0, 7.0, 8.0]];
        reducer.fit(&data).unwrap();
        let result = reducer.transform(&data[0]).unwrap();
        assert_eq!(result.len(), 2);
    }
}
