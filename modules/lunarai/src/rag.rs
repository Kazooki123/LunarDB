use crate::embeddings::EmbeddingGenerator;
use std::error::Error;
use std::collections::HashMap;

pub struct RAGSystem {
    embedding_generator: EmbeddingGenerator,
    document_store: HashMap<String, Vec<f32>>,
}

impl RAGSystem {
    pub fn new(embedding_generator: EmbeddingGenerator) -> Self {
        Self {
            embedding_generator,
            document_store: HashMap::new(),
        }
    }

    pub fn add_document(&mut self, id: String, content: &str) -> Result<(), Box<dyn Error>> {
        let embedding = self.embedding_generator.generate(content)?;
        self.document_store.insert(id, embedding);
        Ok(())
    }

    pub fn retrieve(&self, query: &str, top_k: usize) -> Result<Vec<String>, Box<dyn Error>> {
        let query_embedding = self.embedding_generator.generate(query)?;
        
        let mut similarities: Vec<(String, f32)> = self.document_store
            .iter()
            .map(|(id, embedding)| {
                let similarity = cosine_similarity(&query_embedding, embedding);
                (id.clone(), similarity)
            })
            .collect();

        similarities.sort_by(|a, b| b.1.partial_cmp(&a.1).unwrap());
        let top_results: Vec<String> = similarities.into_iter()
            .take(top_k)
            .map(|(id, _)| id)
            .collect();

        Ok(top_results)
    }
}

fn cosine_similarity(a: &[f32], b: &[f32]) -> f32 {
    let dot_product: f32 = a.iter().zip(b.iter()).map(|(x, y)| x * y).sum();
    let magnitude_a: f32 = a.iter().map(|x| x * x).sum::<f32>().sqrt();
    let magnitude_b: f32 = b.iter().map(|x| x * x).sum::<f32>().sqrt();
    dot_product / (magnitude_a * magnitude_b)
}
