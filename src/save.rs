use std::fs::{File, OpenOptions};
use std::io::{Write, Read, BufReader};
use std::path::Path;
use crate::hashing::hash_sha256;
use std::io::BufRead;

const VALIDATION_HEADER: &str = "LUNARDB_V1";

pub fn save_to_file(data: &str, file_name: &str) -> Result<(), String> {
    let hashed_name = format!("{}.cache", hash_sha256(file_name));
    let path = Path::new(&hashed_name);

    let mut file = File::create(&path).map_err(|e| e.to_string())?;
    file.write_all(format!("{}\n{}", VALIDATION_HEADER, data).as_bytes())
        .map_err(|e| e.to_string())?;

    Ok(())
}

pub fn load_from_file(file_name: &str) -> Result<String, String> {
    let hashed_name = format!("{}.cache", hash_sha256(file_name));
    let path = Path::new(&hashed_name);

    let file = OpenOptions::new().read(true).open(&path).map_err(|e| e.to_string())?;
    let mut reader = BufReader::new(file);

    let mut header = String::new();
    reader.read_line(&mut header).map_err(|e| e.to_string())?;
    if header.trim() != VALIDATION_HEADER {
        return Err("Invalid file format".to_string());
    }

    let mut data = String::new();
    reader.read_to_string(&mut data).map_err(|e| e.to_string())?;
    Ok(data)
}
