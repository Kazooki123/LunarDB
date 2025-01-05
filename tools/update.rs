use std::fs;
use std::io::{self, Error, ErrorKind};
use std::path::Path;
use std::process::Command;
use serde_json::{Value, from_str};

#[derive(Debug)]
struct UpdateConfig {
    current_version: String,
    latest_version: String,
    repo_url: String,
    binary_path: String,
}

fn get_os() -> io::Result<String> {
    let os = std::env::consts::OS;
    match os {
        "linux" => {
            if Path::new("/etc/arch-release").exists() {
                Ok("arch".to_string())
            } else {
                Ok("ubuntu".to_string())
            }
        }
        "windows" => Ok("windows".to_string()),
        unsupported => Err(Error::new(
            ErrorKind::Unsupported,
            format!("Unsupported operating system: {}. LunarDB is only available for Arch Linux, Ubuntu, and Windows.", unsupported)
        ))
    }
}

fn download_file(url: &str, path: &str) -> io::Result<()> {
    println!("Downloading {} to {}", url, path);

    // Create parent directories if they don't exist
    if let Some(parent) = Path::new(path).parent() {
        fs::create_dir_all(parent)?;
    }

    let output = Command::new("curl")
        .args(&["-L", "-o", path, url])
        .output()?;

    if !output.status.success() {
        return Err(Error::new(
            ErrorKind::Other,
            format!("Failed to download file: {}", String::from_utf8_lossy(&output.stderr))
        ));
    }
    Ok(())
}

fn get_update_config() -> io::Result<UpdateConfig> {
    let manifest_path = "manifest.json";
    let manifest_content = fs::read_to_string(manifest_path)
        .map_err(|_| Error::new(ErrorKind::NotFound, "manifest.json not found"))?;

    let manifest: Value = from_str(&manifest_content)
        .map_err(|_| Error::new(ErrorKind::InvalidData, "Invalid manifest.json format"))?;

    let repo_url = "https://github.com/Kazooki123/LunarDB/releases/latest/download";

    // Get latest version
    let latest_version_output = Command::new("curl")
        .args(&["-s", &format!("{}/version.txt", repo_url)])
        .output()?;

    let latest_version = String::from_utf8_lossy(&latest_version_output.stdout)
        .trim()
        .to_string();

    if latest_version.is_empty() {
        return Err(Error::new(ErrorKind::InvalidData, "Failed to fetch latest version"));
    }

    let current_version = manifest["version"]
        .as_str()
        .ok_or_else(|| Error::new(ErrorKind::InvalidData, "Version field missing in manifest.json"))?
        .to_string();

    let target_os = get_os()?;

    let binary_path = manifest["binaries"]
        .as_array()
        .ok_or_else(|| Error::new(ErrorKind::InvalidData, "Invalid binaries field in manifest.json"))?
        .iter()
        .find(|binary| binary["os"].as_str() == Some(&target_os))
        .and_then(|binary| binary["path"].as_str())
        .ok_or_else(|| Error::new(ErrorKind::NotFound, format!("No binary found for OS: {}", target_os)))?
        .to_string();

    Ok(UpdateConfig {
        current_version,
        latest_version,
        repo_url: repo_url.to_string(),
        binary_path,
    })
}

fn update_additional_files(manifest: &Value, repo_url: &str) -> io::Result<()> {
    let files = manifest["files_to_update"]
        .as_array()
        .ok_or_else(|| Error::new(ErrorKind::InvalidData, "Invalid files_to_update field"))?;

    let excluded = manifest["excluded_dirs"]
        .as_array()
        .map(|dirs| dirs.iter().filter_map(|d| d.as_str()).collect::<Vec<_>>())
        .unwrap_or_default();

    for file in files {
        if let Some(path) = file.as_str() {
            // Skip excluded directories
            if excluded.iter().any(|ex| path.starts_with(ex)) {
                println!("Skipping excluded directory: {}", path);
                continue;
            }

            println!("Updating: {}", path);
        }
    }
    Ok(())
}

fn main() -> io::Result<()> {
    println!("Starting LunarDB update process...");

    let config = get_update_config()?;

    println!("Current version: {}", config.current_version);
    println!("Latest version: {}", config.latest_version);

    if config.current_version == config.latest_version {
        println!("LunarDB is already up-to-date!");
        return Ok(());
    }

    println!("Updating to version: {}", config.latest_version);

    // Download and update binary
    let binary_url = format!("{}/{}", config.repo_url, config.binary_path);
    download_file(&binary_url, &config.binary_path)?;

    // Make binary executable on Unix-like systems
    if cfg!(unix) {
        Command::new("chmod")
            .args(&["+x", &config.binary_path])
            .output()?;
    }

    // Read manifest again for updating additional files
    let manifest_content = fs::read_to_string("manifest.json")?;
    let manifest: Value = from_str(&manifest_content)?;

    println!("Updating additional files...");
    update_additional_files(&manifest, &config.repo_url)?;

    println!("Update process completed successfully!");
    Ok(())
}
