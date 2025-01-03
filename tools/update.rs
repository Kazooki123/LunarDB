use std::fs;
use std::io::{self};
use std::process::Command;
use serde_json::{Value, from_str};

fn main() -> io::Result<()> {
  println!("Starting the update process...");

  let manifest_path = "../manifest.json";
  let manifest_content = fs::read_to_string(manifest_path).expect("Failed to read manifest.json. Ensure the file exist..");
  let manifest: Value = from_str(&manifest_content).expect("Failed to parse manifest.json!");

  let current_version = manifest["version"]
    .as_str()
    .expect("Version field missing in manifest.json");

  println!("Current version: {}", current_version);

  let repo_url = "https://github.com/Kazooki123/LunarDB/releases/latest/downloaded";
  let latest_version_output = Command::new("curl")
    .arg("-s")
    .arg(format!("{}/version.txt", repo_url))
    .output()?;

  let latest_version = String::from_utf8_lossy(&latest_version_output.stdout).trim().to_string();

  if current_version == latest_version {
    println!("LunarDB is already up-to-date!");
    return Ok(());
  }

  println!("Updating to version: {}", latest_version);

  let target_os = std::env::consts::OS;

  let binaries = manifest["binaries"]
    .as_array()
    .expect("Invalid binaries field in manifest.json");

  for binary in binaries {
    if binary["os"].as_str().unwrap() == target_os {
      let path = binary["path"]
        .as_str()
        .expect("Invalid binary path in manifest.json");

      println!("Updating binary for OS: {}", target_os);

      let binary_url = format!("{}/{}", repo_url, path);

      let binary_data = Command::new("curl")
        .arg("-L")
        .arg("-o")
        .arg(path)
        .arg(&binary_url)
        .output()?;

      if binary_data.status.success() {
        println!("Successfully updated {}", path);
      } else {
        eprintln!("Failed to update: {}", path)
      }
    }
  }

  println!("Updating other files...");

  for dir in manifest["files_to_update"].as_array().expect("Invalid files_to_update field") {
    let dir_path = dir.as_str().expect("Invalid directory in manifest.json");
    println!("Updating: {}", dir_path);
  }

  println!("Update process completed!!");
  Ok(())
}
