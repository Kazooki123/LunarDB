// lunarstream/build.rs

use std::process::Command;
use std::env;

fn main() {
    let crate_dir = env::var("CARGO_MANIFEST_DIR").unwrap();

    let output = Command::new("cbindgen")
        .arg("--config")
        .arg(format!("{}/cbindgen.toml", crate_dir))
        .arg("--crate")
        .arg("lunarstream")
        .arg("--output")
        .arg(format!("{}/lunarstream.h", crate_dir))
        .output()
        .expect("Failed to run cbindgen");

    if !output.status.success() {
        panic!(
            "cbindgen failed: {}",
            String::from_utf8_lossy(&output.stderr)
        );
    }
}
