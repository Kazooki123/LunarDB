use crate::models::ModelManager;
use crate::embeddings::EmbeddingGenerator;
use crate::rag::RAGSystem;
use std::error::Error;
use std::collections::VecDeque;

const MEMORY_SIZE: usize = 5;

pub struct AIManager {
    model_manager: ModelManager,
    embedding_generator: EmbeddingGenerator,
    rag_system: RAGSystem,
    memory: VecDeque<String>,
}

impl AIManager {
    pub fn new() -> Result<Self, Box<dyn Error>> {
        let embedding_generator = EmbeddingGenerator::new()?;
        Ok(Self {
            model_manager: ModelManager::new()?,
            rag_system: RAGSystem::new(embedding_generator.clone()),
            embedding_generator,
            memory: VecDeque::with_capacity(MEMORY_SIZE),
        })
    }

    pub fn generate_embedding(&self, text: &str) -> Result<Vec<f32>, Box<dyn Error>> {
        self.embedding_generator.generate(text)
    }

    pub fn run_model(&self, model_name: &str, input: &[f32]) -> Result<Vec<f32>, Box<dyn Error>> {
        self.model_manager.run_model(model_name, input)
    }

    pub fn add_document_to_rag(&mut self, id: String, content: &str) -> Result<(), Box<dyn Error>> {
        self.rag_system.add_document(id, content)
    }

    pub fn retrieve_from_rag(&self, query: &str, top_k: usize) -> Result<Vec<String>, Box<dyn Error>> {
        self.rag_system.retrieve(query, top_k)
    }

    pub fn process_input(&mut self, input: &str) -> Result<String, Box<dyn Error>> {
        // Retrieve relevant documents
        let relevant_docs = self.retrieve_from_rag(input, 3)?;

        // Combine input with memory and relevant documents
        let context = self.build_context(input, &relevant_docs);

        // Here you would typically pass the context to your LLM for processing
        // For now, we'll just return a placeholder response
        let response = format!("Processed input: {}. Relevant docs: {:?}", input, relevant_docs);

        // Update memory
        self.update_memory(input, &response);

        Ok(response)
    }

    fn build_context(&self, input: &str, relevant_docs: &[String]) -> String {
        let memory_context = self.memory.iter().cloned().collect::<Vec<String>>().join("\n");
        let docs_context = relevant_docs.join("\n");
        format!("Memory:\n{}\n\nRelevant Documents:\n{}\n\nCurrent Input:\n{}", memory_context, docs_context, input)
    }

    fn update_memory(&mut self, input: &str, response: &str) {
        let memory_entry = format!("Human: {}\nAI: {}", input, response);
        if self.memory.len() >= MEMORY_SIZE {
            self.memory.pop_front();
        }
        self.memory.push_back(memory_entry);
    }
}
