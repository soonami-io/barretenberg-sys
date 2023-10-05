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
            #include <barretenberg/dsl/acir_proofs/c_bind.hpp>
            #include <barretenberg/crypto/blake2s/c_bind.hpp>
            #include <barretenberg/crypto/schnorr/c_bind.hpp>
            #include <barretenberg/ecc/curves/bn254/scalar_multiplication/c_bind.hpp>
            "#,
        )
        .allowlist_function("blake2s_to_field")
        .allowlist_function("acir_proofs_get_solidity_verifier")
        .allowlist_function("acir_proofs_get_exact_circuit_size")
        .allowlist_function("acir_proofs_get_total_circuit_size")
        .allowlist_function("acir_proofs_init_proving_key")
        .allowlist_function("acir_proofs_init_verification_key")
        .allowlist_function("acir_proofs_new_proof")
        .allowlist_function("acir_proofs_verify_proof")
        .allowlist_function("compute_public_key")
        .allowlist_function("construct_signature")
        .allowlist_function("verify_signature")
        .generate()
        .expect("Couldn't generate bindings!");

    let out_path = std::path::PathBuf::from(std::env::var("OUT_DIR").unwrap());

    bindings
        .write_to_file(out_path.join("bindings.rs"))
        .expect("Couldn't write bindings!");
}
