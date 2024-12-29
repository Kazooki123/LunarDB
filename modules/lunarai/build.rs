use std::env;
use std::path::PathBuf;

fn main() {
    let crate_dir = env::var("CARGO_MANIFEST_DIR").unwrap();
    let out_path = PathBuf::from(crate_dir.clone()).join("../include");

    std::fs::create_dir_all(&out_path).unwrap();

    // Generates the bindings
    cbindgen::Builder::new()
        .with_crate(crate_dir)
        .with_language(cbindgen::Language::Cxx)
        .with_pragma_once(true)
        .generate()
        .expect("Unable to generate bindings")
        .write_to_file(out_path.join("lunarai.hpp"));

    println!("cargo:rerun-if-changed=../include/lunarai.hpp");
}
