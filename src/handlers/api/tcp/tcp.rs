use std::net::{TcpListener, TcpStream};
use std::io::{Read, Write};
use std::sync::Arc;
use crate::core::{Core, DataType};
use std::collections::VecDeque;
use std::thread;

pub struct TcpHandler {
    core: Arc<Core>,
    port: u16,
}

impl TcpHandler {
    pub fn new(core: Arc<Core>, port: u16) -> Self {
        TcpHandler { core, port }
    }

    pub fn start(&self) -> Result<(), String> {
        let listener = TcpListener::bind(format!("127.0.0.1:{}", self.port))
            .map_err(|e| format!("Failed to bind: {}", e))?;

        println!("TCP server listening on port {}", self.port);

        for stream in listener.incoming() {
            match stream {
                Ok(stream) => {
                    let core = Arc::clone(&self.core);
                    thread::spawn(move || {
                        if let Err(e) = handle_connection(stream, core) {
                            eprintln!("Connection error: {}", e);
                        }
                    });
                }
                Err(e) => eprintln!("Connection failed: {}", e),
            }
        }

        Ok(())
    }
}

fn handle_connection(mut stream: TcpStream, core: Arc<Core>) -> Result<(), String> {
    let mut buffer = [0; 1024];

    loop {
        let n = stream.read(&mut buffer)
            .map_err(|e| format!("Failed to read from connection: {}", e))?;

        if n == 0 {
            return Ok(());
        }

        let request = String::from_utf8_lossy(&buffer[..n]);
        let response = process_command(&request, &core);

        stream.write_all(response.as_bytes())
            .map_err(|e| format!("Failed to write response: {}", e))?;
    }
}

fn process_command(command: &str, core: &Core) -> String {
    let parts: Vec<&str> = command.trim().split_whitespace().collect();

    if parts.is_empty() {
        return "ERROR: Empty command\n".to_string();
    }

    match parts[0].to_uppercase().as_str() {
        "GET" => {
            if parts.len() != 2 {
                return "ERROR: GET requires 1 argument\n".to_string();
            }
            match core.get(parts[1]) {
                Some(value) => format!("{}\n", value),
                None => "(nil)\n".to_string(),
            }
        }
        "SET" => {
            if parts.len() != 3 {
                return "ERROR: SET requires 2 arguments\n".to_string();
            }
            core.set(
                parts[1].to_string(),
                DataType::String(parts[2].to_string())
            );
            "OK\n".to_string()
        }
        "DEL" => {
            if parts.len() < 2 {
                return "ERROR: DEL requires at least 1 argument\n".to_string();
            }
            let keys: Vec<String> = parts[1..].iter().map(|&s| s.to_string()).collect();
            let count = core.del(&keys);
            format!("{}\n", count)
        }
        "LPUSH" => {
            if parts.len() < 3 {
                return "ERROR: LPUSH requires at least 2 arguments\n".to_string();
            }
            let values: Vec<String> = parts[2..].iter().map(|&s| s.to_string()).collect();
            let count = core.lpush(parts[1].to_string(), values);
            format!("{}\n", count)
        }
        "RPUSH" => {
            if parts.len() < 3 {
                return "ERROR: RPUSH requires at least 2 arguments\n".to_string();
            }
            let values: Vec<String> = parts[2..].iter().map(|&s| s.to_string()).collect();
            let count = core.rpush(parts[1].to_string(), values);
            format!("{}\n", count)
        }
        "LRANGE" => {
            if parts.len() != 4 {
                return "ERROR: LRANGE requires 3 arguments\n".to_string();
            }
            let start: i32 = parts[2].parse().unwrap_or(0);
            let stop: i32 = parts[3].parse().unwrap_or(-1);
            match core.lrange(parts[1], start, stop) {
                Some(list) => format!("{:?}\n", list),
                None => "(nil)\n".to_string(),
            }
        }
        _ => "ERROR: Unknown command\n".to_string(),
    }
}
