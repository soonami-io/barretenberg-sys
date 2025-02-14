#pragma once
#include "barretenberg/proof_system/plookup_tables/plookup_tables.hpp"
#include "barretenberg/stdlib/primitives/uint/uint.hpp"
#include <array>

#include "barretenberg/numeric/bitop/sparse_form.hpp"

#include "../../primitives/circuit_builders/circuit_builders_fwd.hpp"
#include "../../primitives/field/field.hpp"
#include "../../primitives/packed_byte_array/packed_byte_array.hpp"

namespace proof_system::plonk {
namespace stdlib {

namespace blake2s_plookup {

template <typename Composer> byte_array<Composer> blake2s(const byte_array<Composer>& input);

#define BLAKE2S_ULTRA(ULTRA_TYPE) byte_array<ULTRA_TYPE> blake2s(const byte_array<ULTRA_TYPE>& input)

EXTERN_STDLIB_ULTRA_METHOD(BLAKE2S_ULTRA)
} // namespace blake2s_plookup

} // namespace stdlib
} // namespace proof_system::plonk
