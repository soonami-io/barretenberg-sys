// Suppress the flurry of warnings caused by using "C" naming conventions
#![allow(non_upper_case_globals)]
#![allow(non_camel_case_types)]
#![allow(non_snake_case)]

// extern crate link_cplusplus;

// This matches bindgen::Builder output
include!(concat!(env!("OUT_DIR"), "/bindings.rs"));

use acir_proofs::acir_composer::AcirComposer;
pub mod acir_proofs;
pub mod buffer;

fn main() {
    let acir_composer = AcirComposer::new(1024).unwrap();
    drop(acir_composer);
}
