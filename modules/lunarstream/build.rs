// lunarstream/build.rs

use std::env;
use std::path::PathBuf;

fn main() {
    let crate_dir = env::var("CARGO_MANIFEST_DIR").unwrap();
    let out_path = PathBuf::from(crate_dir.clone()).join("../include");

    cbindgen::Builder::new()
        .with_crate(crate_dir)
        .with_language(cbindgen::Language::Cxx)
        .generate()
        .expect("Unable to generate bindings")
        .write_to_file(out_path.join("lunarstream.hpp"));

    // Tell cargo to invalidate the built crate whenever the header changes
    println!("cargo:rerun-if-changed=../include/lunarstream.hpp");
}
