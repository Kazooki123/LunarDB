use std::error::Error;

mod ai;
mod models;
mod embeddings;

fn main() -> Result<(), Box<dyn Error>> {
    println!("LunarAI module initialized");
    
    // Initialize AI components
    let ai_manager = ai::AIManager::new()?;
    
    // Embeddings usage
    let sample_text = "LunarDB is an amazing database!";
    let embedding = ai_manager.generate_embedding(sample_text)?;
    println!("Generated embedding: {:?}", embedding);

    // Add some documents to the RAG system
    ai_manager.add_document_to_rag("doc1".to_string(), "LunarDB is a high-performance database.")?;
    ai_manager.add_document_to_rag("doc2".to_string(), "AI integration enhances database capabilities.")?;

    // Process some inputs
    let response1 = ai_manager.process_input("Tell me about LunarDB.")?;
    println!("Response 1: {}", response1);

    let response2 = ai_manager.process_input("How does AI help databases?")?;
    println!("Response 2: {}", response2);
    
    Ok(())
}
