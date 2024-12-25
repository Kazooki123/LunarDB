use rayon::prelude::*;
use core::result::Result::Ok;
use std::collections::HashMap;

pub struct RAGSystem {
    documents: HashMap<String, Embedding>,
    model: Model,
}

impl RAGSystem {
    pub fn new(model: Model) -> Self {
        Self {
            documents: HashMap::new(),
            model,
        }
    }

    pub async fn add_document(&mut self, id: String, text: &str) -> Result<()> {
        let embedding = self.model.generate_embedding(text).await?;
        self.documents.insert(id, embedding);
        Ok(())
    }

    pub fn search(&self, query_embedding: &Embedding, top_k: usize) -> Vec<(String, f32)> {
        let mut similarities: Vec<_> = self.documents
            .par_iter()
            .map(|(id, emb)| (id.clone(), query_embedding.cosine_similarity(emb)))
            .collect();

        similarities.sort_by(|(_, a), (_, b)| b.partial_cmp(a).unwrap());
        similarities.truncate(top_k);
        similarities
    }
}
