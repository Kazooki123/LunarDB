use crate::models::ModelManager;
use crate::embeddings::EmbeddingGenerator;
use std::error::Error;

pub struct AIManager {
    model_manager: ModelManager,
    embedding_generator: EmbeddingGenerator,
}

impl AIManager {
    pub fn new() -> Result<Self, Box<dyn Error>> {
        Ok(Self {
            model_manager: ModelManager::new()?,
            embedding_generator: EmbeddingGenerator::new()?,
        })
    }

    pub fn generate_embedding(&self, text: &str) -> Result<Vec<f32>, Box<dyn Error>> {
        self.embedding_generator.generate(text)
    }

    pub fn run_model(&self, model_name: &str, input: &[f32]) -> Result<Vec<f32>, Box<dyn Error>> {
        self.model_manager.run_model(model_name, input)
    }
}
