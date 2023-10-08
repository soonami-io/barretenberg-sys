// Suppress the flurry of warnings caused by using "C" naming conventions
#![allow(non_upper_case_globals)]
#![allow(non_camel_case_types)]
#![allow(non_snake_case)]

// extern crate link_cplusplus;

// This matches bindgen::Builder output
include!(concat!(env!("OUT_DIR"), "/bindings.rs"));

use std::io::Read;

use acir_proofs::acir_composer::AcirComposer;
use base64::{Engine as _, alphabet, engine::{self, general_purpose}};
use flate2::read::GzDecoder;

use crate::acir_proofs::acir_composer::get_circuit_sizes;
pub mod acir_proofs;
pub mod buffer;

const BYTECODE: &str = "H4sIAAAAAAAA/7WTMRLEIAhFMYkp9ywgGrHbq6yz5v5H2JkdCyaxC9LgWDw+H9gBwMM91p7fPeOzIKdYjEeMLYdGTB8MpUrCmOohJJQkfYMwN4mSSy0ZC0VudKbCZ4cthqzVrsc/yw28dMZeWmrWerfBexnsxD6hJ7jUufr4GvyZFp8xpG0C14Pd8s/q29vPCBXypvmpDx7sD8opnfqIfsM1RNtxBQAA";

fn main() {
    let acir_buffer = general_purpose::STANDARD.decode(BYTECODE).unwrap();
    let mut decoder = GzDecoder::new(acir_buffer.as_slice());
    let mut acir_buffer_uncompressed = Vec::<u8>::new();
    decoder.read_to_end(&mut acir_buffer_uncompressed).unwrap();
    println!("{:?}", BYTECODE);
    println!("{:?}", hex::encode(acir_buffer.clone()));
    println!("{:?}", hex::encode(acir_buffer_uncompressed.clone()));
    println!("{:?}", acir_buffer.len());
    println!("{:?}", acir_buffer_uncompressed.len());

    let sizes = get_circuit_sizes(&acir_buffer_uncompressed.as_slice());
    println!("{}", sizes.exact);
    println!("{}", sizes.total);
    println!("{}", sizes.subgroup);
}
