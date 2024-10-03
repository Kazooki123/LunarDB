use std::collections::HashMap;
use std::time::Duration;

use crate::vector::LunarVector;

pub fn execute_command(vectors: &mut HashMap<String, LunarVector>, input: &str) -> String {
    let parts: Vec<&str> = input.split_whitespace().collect();
    if parts.is_empty() {
        return "Invalid command".to_string();
    }

    match parts[0].to_uppercase().as_str() {
        "VCREATE" => create_vector_db(vectors, parts),
        "VADD" => add_vector(vectors, parts),
        "VGET" => get_vector(vectors, parts),
        "VSEARCH" => search_similar_vectors(vectors, parts),
        "VLEN" => vector_db_length(vectors, parts),
        "VTTL" => set_vector_ttl(vectors, parts),
        "VCLEANUP" => cleanup_expired_vectors(vectors),
        "HELP" => help(),
        _ => "Unknown command. Type 'HELP' for available commands.".to_string(),
    }
}

fn create_vector_db(vectors: &mut HashMap<String, LunarVector>, parts: Vec<&str>) -> String {
    if parts.len() != 2 {
        return "Usage: VCREATE <key>".to_string();
    }
    let key = parts[1].to_string();
    vectors.insert(key.clone(), LunarVector::new());
    format!("Vector database '{}' created", key)
}

fn add_vector(vectors: &mut HashMap<String, LunarVector>, parts: Vec<&str>) -> String {
    if parts.len() < 4 {
        return "Usage: VADD <db_key> <vector_id> <dim1> <dim2> ...".to_string();
    }
    let db_key = parts[1];
    let vector_id = parts[2];
    let data: Vec<f32> = parts[3..].iter().filter_map(|s| s.parse().ok()).collect();

    if let Some(db) = vectors.get_mut(db_key) {
        db.add(vector_id.to_string(), data);
        "OK".to_string()
    } else {
        format!("Vector database '{}' not found", db_key)
    }
}

fn get_vector(vectors: &HashMap<String, LunarVector>, parts: Vec<&str>) -> String {
    if parts.len() != 3 {
        return "Usage: VGET <db_key> <vector_id>".to_string();
    }
    let db_key = parts[1];
    let vector_id = parts[2];
    if let Some(db) = vectors.get(db_key) {
        if let Some(vector) = db.get(vector_id) {
            format!("{:?}", vector.data())
        } else {
            format!("Vector '{}' not found in database '{}'", vector_id, db_key)
        }
    } else {
        format!("Vector database '{}' not found", db_key)
    }
}

fn search_similar_vectors(vectors: &HashMap<String, LunarVector>, parts: Vec<&str>) -> String {
    if parts.len() < 5 {
        return "Usage: VSEARCH <db_key> <limit> <dim1> <dim2> ...".to_string();
    }
    let db_key = parts[1];
    let limit: usize = match parts[2].parse() {
        Ok(l) => l,
        Err(_) => return "Invalid limit".to_string(),
    };
    let query: Vec<f32> = parts[3..].iter().filter_map(|s| s.parse().ok()).collect();

    if let Some(db) = vectors.get(db_key) {
        let results = db.search_similar(&query, limit);
        format!("{:?}", results)
    } else {
        format!("Vector database '{}' not found", db_key)
    }
}

fn vector_db_length(vectors: &HashMap<String, LunarVector>, parts: Vec<&str>) -> String {
    if parts.len() != 2 {
        return "Usage: VLEN <db_key>".to_string();
    }
    let db_key = parts[1];
    if let Some(db) = vectors.get(db_key) {
        db.len().to_string()
    } else {
        format!("Vector database '{}' not found", db_key)
    }
}

fn set_vector_ttl(vectors: &mut HashMap<String, LunarVector>, parts: Vec<&str>) -> String {
    if parts.len() != 4 {
        return "Usage: VTTL <db_key> <vector_id> <seconds>".to_string();
    }
    let db_key = parts[1];
    let vector_id = parts[2];
    let seconds: u64 = match parts[3].parse() {
        Ok(s) => s,
        Err(_) => return "Invalid TTL value".to_string(),
    };
    if let Some(db) = vectors.get_mut(db_key) {
        if db.set_ttl(vector_id, Duration::from_secs(seconds)) {
            "OK".to_string()
        } else {
            format!("Vector '{}' not found in database '{}'", vector_id, db_key)
        }
    } else {
        format!("Vector database '{}' not found", db_key)
    }
}

fn cleanup_expired_vectors(vectors: &mut HashMap<String, LunarVector>) -> String {
    for db in vectors.values_mut() {
        db.cleanup_expired();
    }
    "Expired vectors cleaned up".to_string()
}

fn help() -> String {
    "Available commands:
    VCREATE <db_key> - Create a new vector database
    VADD <db_key> <vector_id> <dim1> <dim2> ... - Add a vector to the database
    VGET <db_key> <vector_id> - Get a vector from the database
    VSEARCH <db_key> <limit> <dim1> <dim2> ... - Search for similar vectors
    VLEN <db_key> - Get the number of vectors in the database
    VTTL <db_key> <vector_id> <seconds> - Set the TTL for a vector
    VCLEANUP - Remove expired vectors from all databases
    QUIT - Exit the program
    HELP - Show this help message".to_string()
}
