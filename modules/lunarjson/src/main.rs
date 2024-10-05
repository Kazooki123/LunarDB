mod json;

use json::{LunarJSON, parse_json};
use std::io::{self, Write};

fn main() {
    let mut lunar_json = LunarJSON::new();

    println!("Welcome to LunarJSON!");
    println!("Type 'help' for available commands.");

    loop {
        print!("> ");
        io::stdout().flush().unwrap();

        let mut input = String::new();
        io::stdin().read_line(&mut input).unwrap();
        let input = input.trim();

        let parts: Vec<&str> = input.splitn(2, ' ').collect();
        let command = parts[0].to_lowercase();

        match command.as_str() {
            "set" => {
                if parts.len() != 2 {
                    println!("Usage: set <json_string>");
                    continue;
                }
                match parse_json(parts[1]) {
                    Ok(json) => {
                        lunar_json.set("default", json).unwrap();
                        println!("JSON set successfully");
                    }
                    Err(e) => println!("Error parsing JSON: {}", e),
                }
            }
            "get" => {
                if let Some(json) = lunar_json.get("default") {
                    println!("{}", json);
                } else {
                    println!("No JSON data set");
                }
            }
            "get_field" => {
                if parts.len() != 2 {
                    println!("Usage: get_field <path>");
                    continue;
                }
                if let Some(value) = lunar_json.get_field("default", parts[1]) {
                    println!("{}", value);
                } else {
                    println!("Field not found");
                }
            }
            "update_field" => {
                if parts.len() != 2 {
                    println!("Usage: update_field <path> <json_value>");
                    continue;
                }
                let field_parts: Vec<&str> = parts[1].splitn(2, ' ').collect();
                if field_parts.len() != 2 {
                    println!("Usage: update_field <path> <json_value>");
                    continue;
                }
                match parse_json(field_parts[1]) {
                    Ok(json) => {
                        match lunar_json.update_field("default", field_parts[0], json) {
                            Ok(_) => println!("Field updated successfully"),
                            Err(e) => println!("Error updating field: {}", e),
                        }
                    }
                    Err(e) => println!("Error parsing JSON: {}", e),
                }
            }
            "delete_field" => {
                if parts.len() != 2 {
                    println!("Usage: delete_field <path>");
                    continue;
                }
                match lunar_json.delete_field("default", parts[1]) {
                    Ok(_) => println!("Field deleted successfully"),
                    Err(e) => println!("Error deleting field: {}", e),
                }
            }
            "push_to_array" => {
                if parts.len() != 2 {
                    println!("Usage: push_to_array <path> <json_value>");
                    continue;
                }
                let array_parts: Vec<&str> = parts[1].splitn(2, ' ').collect();
                if array_parts.len() != 2 {
                    println!("Usage: push_to_array <path> <json_value>");
                    continue;
                }
                match parse_json(array_parts[1]) {
                    Ok(json) => {
                        match lunar_json.push_to_array("default", array_parts[0], json) {
                            Ok(_) => println!("Value pushed to array successfully"),
                            Err(e) => println!("Error pushing to array: {}", e),
                        }
                    }
                    Err(e) => println!("Error parsing JSON: {}", e),
                }
            }
            "pop_from_array" => {
                if parts.len() != 2 {
                    println!("Usage: pop_from_array <path>");
                    continue;
                }
                match lunar_json.pop_from_array("default", parts[1]) {
                    Ok(Some(value)) => println!("Popped value: {}", value),
                    Ok(None) => println!("Array is empty"),
                    Err(e) => println!("Error popping from array: {}", e),
                }
            }
            "help" => {
                println!("Available commands:");
                println!("  set <json_string>");
                println!("  get");
                println!("  get_field <path>");
                println!("  update_field <path> <json_value>");
                println!("  delete_field <path>");
                println!("  push_to_array <path> <json_value>");
                println!("  pop_from_array <path>");
                println!("  quit");
            }
            "quit" => break,
            _ => println!("Unknown command. Type 'help' for available commands."),
        }
    }

    println!("Goodbye!");
}
