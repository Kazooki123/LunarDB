use std::collections::HashMap;
use std::time::Duration;

use crate::vector::LunarVector;

pub fn execute_command(vectors: &mut HashMap<String, LunarVector>, input: &str) -> String {
    let parts: Vec<&str> = input.split_whitespace().collect();
    if parts.is_empty() {
        return "Invalid command".to_string();
    }

    match parts[0].to_uppercase().as_str() {
        "VCREATE" => create_vector(vectors, parts),
        "VPUSH" => push_to_vector(vectors, parts),
        "VPOP" => pop_from_vector(vectors, parts),
        "VGET" => get_from_vector(vectors, parts),
        "VLEN" => vector_length(vectors, parts),
        "VTTL" => set_vector_ttl(vectors, parts),
        "HELP" => help(),
        _ => "Unknown command. Type 'HELP' for available commands.".to_string(),
    }
}

fn create_vector(vectors: &mut HashMap<String, LunarVector>, parts: Vec<&str>) -> String {
    if parts.len() != 2 {
        return "Usage: VCREATE <key>".to_string();
    }
    let key = parts[1].to_string();
    vectors.insert(key.clone(), LunarVector::new());
    format!("Vector '{}' created", key)
}

fn push_to_vector(vectors: &mut HashMap<String, LunarVector>, parts: Vec<&str>) -> String {
    if parts.len() != 3 {
        return "Usage: VPUSH <key> <value>".to_string();
    }
    let key = parts[1];
    let value = parts[2];
    if let Some(vector) = vectors.get_mut(key) {
        vector.push(value.to_string());
        "OK".to_string()
    } else {
        format!("Vector '{}' not found", key)
    }
}

fn pop_from_vector(vectors: &mut HashMap<String, LunarVector>, parts: Vec<&str>) -> String {
    if parts.len() != 2 {
        return "Usage: VPOP <key>".to_string();
    }
    let key = parts[1];
    if let Some(vector) = vectors.get_mut(key) {
        if let Some(value) = vector.pop() {
            value
        } else {
            "(nil)".to_string()
        }
    } else {
        format!("Vector '{}' not found", key)
    }
}

fn get_from_vector(vectors: &HashMap<String, LunarVector>, parts: Vec<&str>) -> String {
    if parts.len() != 3 {
        return "Usage: VGET <key> <index>".to_string();
    }
    let key = parts[1];
    let index: usize = match parts[2].parse() {
        Ok(i) => i,
        Err(_) => return "Invalid index".to_string(),
    };
    if let Some(vector) = vectors.get(key) {
        if let Some(value) = vector.get(index) {
            value.clone()
        } else {
            "(nil)".to_string()
        }
    } else {
        format!("Vector '{}' not found", key)
    }
}

fn vector_length(vectors: &HashMap<String, LunarVector>, parts: Vec<&str>) -> String {
    if parts.len() != 2 {
        return "Usage: VLEN <key>".to_string();
    }
    let key = parts[1];
    if let Some(vector) = vectors.get(key) {
        vector.len().to_string()
    } else {
        format!("Vector '{}' not found", key)
    }
}

fn set_vector_ttl(vectors: &mut HashMap<String, LunarVector>, parts: Vec<&str>) -> String {
    if parts.len() != 3 {
        return "Usage: VTTL <key> <seconds>".to_string();
    }
    let key = parts[1];
    let seconds: u64 = match parts[2].parse() {
        Ok(s) => s,
        Err(_) => return "Invalid TTL value".to_string(),
    };
    if let Some(vector) = vectors.get_mut(key) {
        vector.set_ttl(Duration::from_secs(seconds));
        "OK".to_string()
    } else {
        format!("Vector '{}' not found", key)
    }
}

fn help() -> String {
    "Available commands:
    VCREATE <key> - Create a new vector
    VPUSH <key> <value> - Push a value to the end of the vector
    VPOP <key> - Remove and return the last element from the vector
    VGET <key> <index> - Get the value at the specified index
    VLEN <key> - Get the length of the vector
    VTTL <key> <seconds> - Set the TTL for the vector
    QUIT - Exit the program
    HELP - Show this help message".to_string()
}