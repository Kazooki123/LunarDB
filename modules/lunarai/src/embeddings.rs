// use rust_bert::pipelines::sentence_embeddings::{SentenceEmbeddingsBuilder, SentenceEmbeddingsModel};
use std::error::Error;

#[derive(Debug, Clone)]
pub struct EmbeddingGenerator {
    model: SentenceEmbeddingsModel,
}

impl EmbeddingGenerator {
    pub fn new() -> Result<Self, Box<dyn Error>> {
        let model = SentenceEmbeddingsBuilder::remote(rust_bert::pipelines::sentence_embeddings::SentenceEmbeddingsModelType::AllMiniLmL12V2)
            .create_model()?;
        Ok(Self { model })
    }

    pub fn generate(&self, text: &str) -> Result<Vec<f32>, Box<dyn Error>> {
        let embeddings = self.model.encode(&[text])?;
        Ok(embeddings[0].clone())
    }
}
