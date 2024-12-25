use core::result::Result::Ok;

use anyhow::Result;
use rust_bert::pipelines::sentence_embeddings::{
    SentenceEmbeddingsBuilder, SentenceEmbeddingsModel,
};
use cached::proc_macro::cached;

pub struct Model {
    model: SentenceEmbeddingsModel,
    name: String,
}

impl Model {
    pub fn new(name: &str) -> Result<Self> {
        let model = SentenceEmbeddingsBuilder::local(name)
            .create_model()?;
        Ok(Self {
            model,
            name: name.to_string(),
        })
    }

    #[cached(size = 100, result = true)]
    pub async fn generate_embedding(&self, text: &str) -> Result<Embedding> {
        let embeddings = self.model.encode(&[text])?;
        let vector = Array1::from_vec(embeddings[0].clone());
        Ok(Embedding::new(vector, self.name.clone()))
    }
}
