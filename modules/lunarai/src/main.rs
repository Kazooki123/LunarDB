use std::error::Error;

mod ai;
mod models;
mod embeddings;

fn main() -> Result<(), Box<dyn Error>> {
    println!("LunarAI module initialized");
    
    // Initialize AI components
    let ai_manager = ai::AIManager::new()?;
    
    // Example usage
    let sample_text = "LunarDB is an amazing database!";
    let embedding = ai_manager.generate_embedding(sample_text)?;
    println!("Generated embedding: {:?}", embedding);
    
    Ok(())
}
