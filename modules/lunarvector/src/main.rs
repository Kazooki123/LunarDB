use std::collections::HashMap;

mod vector;
mod commands;

use vector::LunarVector;
use commands::execute_command;

fn main() {
    let mut vectors: HashMap<String, LunarVector> = HashMap::new();

    println!("Welcome to LunarVector module for LunarDB!");
    println!("Type 'HELP' for available commands.");

    loop {
        let mut input = String::new();
        std::io::stdin().read_line(&mut input).unwrap();
        let input = input.trim();

        if input.eq_ignore_ascii_case("QUIT") {
            break;
        }

        let result = execute_command(&mut vectors, input);
        println!("{}", result);
    }

    println!("Goodbye!");
}