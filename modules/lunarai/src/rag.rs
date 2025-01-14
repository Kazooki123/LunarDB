use crate::embeddings::EmbeddingEngine;
use crate::LunarAIError;
use hashbrown::HashMap;

#[derive(Debug, Clone)]
pub struct Document {
    pub id: String,
    pub content: String,
    pub metadata: HashMap<String, String>,
    pub embedding: Option<Vec<f32>>,
}

pub struct RAGEngine {
    embedding_engine: EmbeddingEngine,
    documents: HashMap<String, Document>,
    chunk_size: usize,
    overlap: usize,
}

impl RAGEngine {
    pub fn new() -> Self {
        Self {
            embedding_engine: EmbeddingEngine::new(),
            documents: HashMap::new(),
            chunk_size: 512,
            overlap: 50,
        }
    }

    // Add a document to the RAG system
    pub fn add_document(&mut self, doc: Document) -> Result<(), LunarAIError> {
        let chunks = self.chunk_document(&doc.content)?;
        let mut embedded_chunks = Vec::new();

        for chunk in chunks {
            let embedding = self.embedding_engine.generate_embedding(&chunk)?;
            embedded_chunks.push(embedding);
        }

        let mut doc = doc;
        doc.embedding = Some(embedded_chunks.concat());
        self.documents.insert(doc.id.clone(), doc);
        Ok(())
    }

    // Retrieve relevant documents based on a query
    pub fn retrieve(&self, query: &str, top_k: usize) -> Result<Vec<&Document>, LunarAIError> {
        let query_embedding = self.embedding_engine.generate_embedding(query)?;

        let mut similarities: Vec<_> = self.documents
            .values()
            .filter_map(|doc| {
                doc.embedding.as_ref().map(|emb| {
                    self.embedding_engine
                        .similarity(&query_embedding, emb)
                        .map(|sim| (doc, sim))
                })
            })
            .collect::<Result<Vec<_>, _>>()?;

        similarities.sort_by(|(_, a), (_, b)| b.partial_cmp(a).unwrap_or(std::cmp::Ordering::Equal));
        Ok(similarities.into_iter().take(top_k).map(|(doc, _)| doc).collect())
    }

    // Split document into chunks with overlap
    fn chunk_document(&self, content: &str) -> Result<Vec<String>, LunarAIError> {
        let words: Vec<&str> = content.split_whitespace().collect();
        let mut chunks = Vec::new();
        let mut start = 0;

        while start < words.len() {
            let end = (start + self.chunk_size).min(words.len());
            let chunk = words[start..end].join(" ");
            chunks.push(chunk);
            start += self.chunk_size - self.overlap;
        }

        Ok(chunks)
    }

    // Update chunk size and overlap
    pub fn configure(&mut self, chunk_size: usize, overlap: usize) -> Result<(), LunarAIError> {
        if overlap >= chunk_size {
            return Err(LunarAIError::RAGError(
                "Overlap must be smaller than chunk size".to_string(),
            ));
        }
        self.chunk_size = chunk_size;
        self.overlap = overlap;
        Ok(())
    }

    // Remove a document from the RAG system
    pub fn remove_document(&mut self, id: &str) -> Option<Document> {
        self.documents.remove(id)
    }

    // Clear all documents
    pub fn clear(&mut self) {
        self.documents.clear();
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_document_addition_and_retrieval() {
        let mut rag = RAGEngine::new();

        let doc = Document {
            id: "test1".to_string(),
            content: "This is a test document".to_string(),
            metadata: HashMap::new(),
            embedding: None,
        };

        rag.add_document(doc).unwrap();
        let results = rag.retrieve("test document", 1).unwrap();
        assert_eq!(results.len(), 1);
        assert_eq!(results[0].id, "test1");
    }
}
