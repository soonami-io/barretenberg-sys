#include "barretenberg/common/wasm_export.hpp"
#include <barretenberg/common/serialize.hpp>
#include <cstdint>

extern "C"
{
    const char* examples_simple_create_and_verify_proof(bool* valid);
}
