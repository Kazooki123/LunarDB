use crate::{Embedding, EmbeddingError, AIPipeline};
use rayon::prelude::*;
use core::{option::Option::Some, result::Result::Ok};
use std::collections::HashMap;
use serde::{Serialize, Deserialize};

#[derive(Debug, Serialize, Deserialize)]
pub struct Document {
    pub id: String,
    pub content: String,
    pub embedding: Option<Embedding>,
}

#[derive(Debug)]
pub struct RAGSystem {
    documents: HashMap<String, Document>,
    pipeline: AIPipeline,
}

impl RAGSystem {
    pub fn new(pipeline: AIPipeline) -> Self {
        Self {
            documents: HashMap::new(),
            pipeline,
        }
    }

    pub async fn add_document(&mut self, id: String, content: String) -> Result<(), EmbeddingError> {
        let embedding = self.pipeline.generate_embedding(&content).await?;

        let document = Document {
            id: id.clone(),
            content,
            embedding: Some(embedding),
        };

        self.documents.insert(id, document);
        Ok(())
    }

    pub async fn search(&self, query: &str, top_k: usize) -> Result<Vec<(String, f32)>, EmbeddingError> {
        let query_embedding = self.pipeline.generate_embedding(query).await?;

        let mut similarities: Vec<_> = self.documents
            .par_iter()
            .filter_map(|(id, doc)| {
                doc.embedding.as_ref().map(|emb| {
                    (id.clone(), query_embedding.cosine_similarity(emb))
                })
            })
            .collect();

        similarities.sort_by(|(_, a), (_, b)| b.partial_cmp(a).unwrap());
        similarities.truncate(top_k);

        Ok(similarities)
    }
}
