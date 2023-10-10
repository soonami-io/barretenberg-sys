fn main() {
    println!("cargo:rerun-if-changed=build.rs");
    // Tell cargo to look for shared libraries in the specified directory
    println!("cargo:rustc-link-search=./lib");
    // Tell cargo to tell rustc to link static barretenberg
    println!("cargo:rustc-link-lib=static=barretenberg");
    println!("cargo:rustc-link-lib=stdc++");

    // The bindgen::Builder is the main entry point
    // to bindgen, and lets you build up options for
    // the resulting bindings.
    let bindings = bindgen::Builder::default()
        // Clang args so that we can compile C++ with C++20
        .clang_args(&["-std=gnu++20", "-xc++"])
        .clang_args(&["-I./"])
        .header_contents(
            "wrapper.hpp",
            r#"
            #include <barretenberg/dsl/acir_proofs/acir_proofs.hpp>
            #include <barretenberg/srs/c_bind.hpp>
            #include <barretenberg/examples/c_bind.hpp>
            "#,
        )
        .allowlist_function("acir_get_circuit_sizes")
        .allowlist_function("acir_new_acir_composer")
        .allowlist_function("acir_delete_acir_composer")
        .allowlist_function("acir_create_circuit")
        .allowlist_function("acir_init_proving_key")
        .allowlist_function("acir_create_proof")
        .allowlist_function("acir_load_verification_key")
        .allowlist_function("acir_init_verification_key")
        .allowlist_function("acir_get_verification_key")
        .allowlist_function("acir_verify_proof")
        .allowlist_function("acir_get_solidity_verifier")
        .allowlist_function("acir_serialize_proof_into_fields")
        .allowlist_function("acir_serialize_verification_key_into_fields")
        .allowlist_function("srs_init_srs")
        .allowlist_function("examples_simple_create_and_verify_proof")
        .generate()
        .expect("Couldn't generate bindings!");

    let out_path = std::path::PathBuf::from(std::env::var("OUT_DIR").unwrap());

    bindings
        .write_to_file(out_path.join("bindings.rs"))
        .expect("Couldn't write bindings!");
}
