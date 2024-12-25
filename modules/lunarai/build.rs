// build.rs
use std::env;
use std::path::PathBuf;

fn main() {
    let crate_dir = env::var("CARGO_MANIFEST_DIR").unwrap();

    let config = cbindgen::Config {
        language: cbindgen::Language::Cpp,
        namespace: core::option::Option::Some(String::from("lunar")),
        namespaces: vec!["lunar".to_string()],
        braces: cbindgen::Braces::SameLine,
        cpp_compat: true,
        pragma_once: true,
        documentation: true,
        ..Default::default()
    };

    cbindgen::Builder::new()
        .with_crate(crate_dir)
        .with_config(config)
        .generate()
        .expect("Unable to generate bindings")
        .write_to_file("include/lunar_embeddings.hpp");
}
