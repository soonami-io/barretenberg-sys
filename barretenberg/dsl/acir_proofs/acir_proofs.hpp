#include <barretenberg/common/serialize.hpp>
#include <barretenberg/common/wasm_export.hpp>
#include <barretenberg/ecc/curves/bn254/fr.hpp>
#include <cstddef>
#include <cstdint>

using namespace barretenberg;

extern "C" {

__attribute__((visibility("default"))) void acir_get_circuit_sizes(uint8_t const* constraint_system_buf,
                                        uint32_t* exact,
                                        uint32_t* total,
                                        uint32_t* subgroup);

__attribute__((visibility("default"))) void acir_new_acir_composer(uint32_t const* size_hint, out_ptr out);

__attribute__((visibility("default"))) void acir_delete_acir_composer(in_ptr acir_composer_ptr);

__attribute__((visibility("default"))) void acir_create_circuit(in_ptr acir_composer_ptr,
                                     uint8_t const* constraint_system_buf,
                                     uint32_t const* size_hint);

__attribute__((visibility("default"))) void acir_init_proving_key(in_ptr acir_composer_ptr, uint8_t const* constraint_system_buf);

/**
 * It would have been nice to just hold onto the constraint_system in the acir_composer, but we can't waste the
 * memory. Being able to reuse the underlying Composer would help as well. But, given the situation, we just have
 * to pass it in everytime.
 */
__attribute__((visibility("default"))) void acir_create_proof(in_ptr acir_composer_ptr,
                                   uint8_t const* constraint_system_buf,
                                   uint8_t const* witness_buf,
                                   bool const* is_recursive,
                                   uint8_t** out);

__attribute__((visibility("default"))) void acir_load_verification_key(in_ptr acir_composer_ptr, uint8_t const* vk_buf);

__attribute__((visibility("default"))) void acir_init_verification_key(in_ptr acir_composer_ptr);

__attribute__((visibility("default"))) void acir_get_verification_key(in_ptr acir_composer_ptr, uint8_t** out);

__attribute__((visibility("default"))) void acir_verify_proof(in_ptr acir_composer_ptr,
                                   uint8_t const* proof_buf,
                                   bool const* is_recursive,
                                   bool* result);

__attribute__((visibility("default"))) void acir_get_solidity_verifier(in_ptr acir_composer_ptr, out_str_buf out);

__attribute__((visibility("default"))) void acir_serialize_proof_into_fields(in_ptr acir_composer_ptr,
                                                  uint8_t const* proof_buf,
                                                  uint32_t const* num_inner_public_inputs,
                                                  fr::vec_out_buf out);

__attribute__((visibility("default"))) void acir_serialize_verification_key_into_fields(in_ptr acir_composer_ptr,
                                                             fr::vec_out_buf out_vkey,
                                                             fr::out_buf out_key_hash);

}