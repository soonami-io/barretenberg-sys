// Suppress the flurry of warnings caused by using "C" naming conventions
#![allow(non_upper_case_globals)]
#![allow(non_camel_case_types)]
#![allow(non_snake_case)]

// extern crate link_cplusplus;

// This matches bindgen::Builder output
include!(concat!(env!("OUT_DIR"), "/bindings.rs"));

use std::io::Read;

use base64::{engine::general_purpose, Engine as _};
use flate2::read::GzDecoder;

use crate::{acir_proofs::acir_composer::{get_circuit_sizes, AcirComposer}, srs::{netsrs::NetSrs, srs_init}};
pub mod acir_proofs;
pub mod buffer;
pub mod srs;

const BYTECODE: &str = "H4sIAAAAAAAA/7WTMRLEIAhFMYkp9ywgGrHbq6yz5v5H2JkdCyaxC9LgWDw+H9gBwMM91p7fPeOzIKdYjEeMLYdGTB8MpUrCmOohJJQkfYMwN4mSSy0ZC0VudKbCZ4cthqzVrsc/yw28dMZeWmrWerfBexnsxD6hJ7jUufr4GvyZFp8xpG0C14Pd8s/q29vPCBXypvmpDx7sD8opnfqIfsM1RNtxBQAA";
const SOLVEDWITNESS: &str = "05000000000000000100000040000000000000003030303030303030303030303030303030303030303030303030303030303030303030303030303030303030303030303030303030303030303030303030303302000000400000000000000030303030303030303030303030303030303030303030303030303030303030303030303030303030303030303030303030303030303030303030303030303034030000004000000000000000333036343465373265313331613032396238353034356236383138313538356432383333653834383739623937303931343365316635393366303030303030300400000040000000000000003330363434653732653133316130323962383530343562363831383135383564323833336538343837396239373039313433653166353933663030303030303005000000400000000000000030303030303030303030303030303030303030303030303030303030303030303030303030303030303030303030303030303030303030303030303030303030";

fn main() {
    // let acir_buffer = general_purpose::STANDARD.decode(BYTECODE).unwrap();
    // let mut decoder = GzDecoder::new(acir_buffer.as_slice());
    // let mut acir_buffer_uncompressed = Vec::<u8>::new();
    // decoder.read_to_end(&mut acir_buffer_uncompressed).unwrap();
    
    // let circuit_size = get_circuit_sizes(&acir_buffer_uncompressed);
    // let log_value = (circuit_size.total as f64).log2().ceil() as u32;
    // let subgroup_size = 2u32.pow(log_value);
    let srs = NetSrs::new(2_u32.pow(19) + 1);
    srs_init(&srs.g1_data, srs.num_points, &srs.g2_data);
    println!("{}", AcirComposer::simple_create_and_verify_proof());
    // let acir_composer = AcirComposer::new(subgroup_size).unwrap();
    // let witness = hex::decode(SOLVEDWITNESS).unwrap();

    // let proof = acir_composer.create_proof(&acir_buffer_uncompressed, &witness, false).unwrap();
    
    // acir_composer.init_proving_key(&acir_buffer_uncompressed);
    // let verdict = acir_composer.verify_proof(&proof, false);

    // println!("{:?}", hex::encode(proof));
    // println!("{:?}", verdict);
}
