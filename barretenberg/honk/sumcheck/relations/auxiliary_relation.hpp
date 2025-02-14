#pragma once
#include <array>
#include <tuple>

#include "../polynomials/univariate.hpp"
#include "barretenberg/numeric/uint256/uint256.hpp"
#include "relation_parameters.hpp"
#include "relation_types.hpp"

namespace proof_system::honk::sumcheck {

template <typename FF> class AuxiliaryRelationBase {
  public:
    // 1 + polynomial degree of this relation
    static constexpr size_t RELATION_LENGTH = 6;

    static constexpr size_t LEN_1 = 6; // auxiliary sub-relation
    static constexpr size_t LEN_2 = 6; // ROM consistency sub-relation 1
    static constexpr size_t LEN_3 = 6; // ROM consistency sub-relation 2
    static constexpr size_t LEN_4 = 6; // RAM consistency sub-relation 1
    static constexpr size_t LEN_5 = 6; // RAM consistency sub-relation 2
    static constexpr size_t LEN_6 = 6; // RAM consistency sub-relation 3
    template <template <size_t...> typename AccumulatorTypesContainer>
    using AccumulatorTypesBase = AccumulatorTypesContainer<LEN_1, LEN_2, LEN_3, LEN_4, LEN_5, LEN_6>;

    /**
     * @brief Expression for the generalized permutation sort gate.
     * @details The following explanation is reproduced from the Plonk analog 'plookup_auxiliary_widget':
     * Adds contributions for identities associated with several custom gates:
     *  * RAM/ROM read-write consistency check
     *  * RAM timestamp difference consistency check
     *  * RAM/ROM index difference consistency check
     *  * Bigfield product evaluation (3 in total)
     *  * Bigfield limb accumulation (2 in total)
     *
     * Multiple selectors are used to 'switch' aux gates on/off according to the following pattern:
     *
     * | gate type                    | q_aux | q_1 | q_2 | q_3 | q_4 | q_m | q_c | q_arith |
     * | ---------------------------- | ----- | --- | --- | --- | --- | --- | --- | ------  |
     * | Bigfield Limb Accumulation 1 | 1     | 0   | 0   | 1   | 1   | 0   | --- | 0       |
     * | Bigfield Limb Accumulation 2 | 1     | 0   | 0   | 1   | 0   | 1   | --- | 0       |
     * | Bigfield Product 1           | 1     | 0   | 1   | 1   | 0   | 0   | --- | 0       |
     * | Bigfield Product 2           | 1     | 0   | 1   | 0   | 1   | 0   | --- | 0       |
     * | Bigfield Product 3           | 1     | 0   | 1   | 0   | 0   | 1   | --- | 0       |
     * | RAM/ROM access gate          | 1     | 1   | 0   | 0   | 0   | 1   | --- | 0       |
     * | RAM timestamp check          | 1     | 1   | 0   | 0   | 1   | 0   | --- | 0       |
     * | ROM consistency check        | 1     | 1   | 1   | 0   | 0   | 0   | --- | 0       |
     * | RAM consistency check        | 1     | 0   | 0   | 0   | 0   | 0   | 0   | 1       |
     *
     * N.B. The RAM consistency check identity is degree 3. To keep the overall quotient degree at <=5, only 2 selectors
     * can be used to select it.
     *
     * N.B.2 The q_c selector is used to store circuit-specific values in the RAM/ROM access gate
     *
     * @param evals transformed to `evals + C(extended_edges(X)...)*scaling_factor`
     * @param extended_edges an std::array containing the fully extended Univariate edges.
     * @param parameters contains beta, gamma, and public_input_delta, ....
     * @param scaling_factor optional term to scale the evaluation before adding to evals.
     */
    template <typename AccumulatorTypes>
    inline static void add_edge_contribution_impl(typename AccumulatorTypes::Accumulators& accumulators,
                                                  const auto& extended_edges,
                                                  const RelationParameters<FF>& relation_parameters,
                                                  const FF& scaling_factor)
    {
        // OPTIMIZATION?: Karatsuba in general, at least for some degrees?
        //       See https://hackmd.io/xGLuj6biSsCjzQnYN-pEiA?both

        const auto& eta = relation_parameters.eta;

        // All subrelations have the same length so we use the same length view for all calculations
        using View = typename std::tuple_element<0, typename AccumulatorTypes::AccumulatorViews>::type;
        auto w_1 = View(extended_edges.w_l);
        auto w_2 = View(extended_edges.w_r);
        auto w_3 = View(extended_edges.w_o);
        auto w_4 = View(extended_edges.w_4);
        auto w_1_shift = View(extended_edges.w_l_shift);
        auto w_2_shift = View(extended_edges.w_r_shift);
        auto w_3_shift = View(extended_edges.w_o_shift);
        auto w_4_shift = View(extended_edges.w_4_shift);

        auto q_1 = View(extended_edges.q_l);
        auto q_2 = View(extended_edges.q_r);
        auto q_3 = View(extended_edges.q_o);
        auto q_4 = View(extended_edges.q_4);
        auto q_m = View(extended_edges.q_m);
        auto q_c = View(extended_edges.q_c);
        auto q_arith = View(extended_edges.q_arith);
        auto q_aux = View(extended_edges.q_aux);

        const FF LIMB_SIZE(uint256_t(1) << 68);
        const FF SUBLIMB_SHIFT(uint256_t(1) << 14);

        /**
         * Non native field arithmetic gate 2
         *
         *             _                                                                               _
         *            /   _                   _                               _       14                \
         * q_2 . q_4 |   (w_1 . w_2) + (w_1 . w_2) + (w_1 . w_4 + w_2 . w_3 - w_3) . 2    - w_3 - w_4   |
         *            \_                                                                               _/
         *
         **/
        auto limb_subproduct = w_1 * w_2_shift + w_1_shift * w_2;
        auto non_native_field_gate_2 = (w_1 * w_4 + w_2 * w_3 - w_3_shift);
        non_native_field_gate_2 *= LIMB_SIZE;
        non_native_field_gate_2 -= w_4_shift;
        non_native_field_gate_2 += limb_subproduct;
        non_native_field_gate_2 *= q_4;

        limb_subproduct *= LIMB_SIZE;
        limb_subproduct += (w_1_shift * w_2_shift);
        auto non_native_field_gate_1 = limb_subproduct;
        non_native_field_gate_1 -= (w_3 + w_4);
        non_native_field_gate_1 *= q_3;

        auto non_native_field_gate_3 = limb_subproduct;
        non_native_field_gate_3 += w_4;
        non_native_field_gate_3 -= (w_3_shift + w_4_shift);
        non_native_field_gate_3 *= q_m;

        auto non_native_field_identity = non_native_field_gate_1 + non_native_field_gate_2 + non_native_field_gate_3;
        non_native_field_identity *= q_2;

        auto limb_accumulator_1 = w_2_shift * SUBLIMB_SHIFT;
        limb_accumulator_1 += w_1_shift;
        limb_accumulator_1 *= SUBLIMB_SHIFT;
        limb_accumulator_1 += w_3;
        limb_accumulator_1 *= SUBLIMB_SHIFT;
        limb_accumulator_1 += w_2;
        limb_accumulator_1 *= SUBLIMB_SHIFT;
        limb_accumulator_1 += w_1;
        limb_accumulator_1 -= w_4;
        limb_accumulator_1 *= q_4;

        auto limb_accumulator_2 = w_3_shift * SUBLIMB_SHIFT;
        limb_accumulator_2 += w_2_shift;
        limb_accumulator_2 *= SUBLIMB_SHIFT;
        limb_accumulator_2 += w_1_shift;
        limb_accumulator_2 *= SUBLIMB_SHIFT;
        limb_accumulator_2 += w_4;
        limb_accumulator_2 *= SUBLIMB_SHIFT;
        limb_accumulator_2 += w_3;
        limb_accumulator_2 -= w_4_shift;
        limb_accumulator_2 *= q_m;

        auto limb_accumulator_identity = limb_accumulator_1 + limb_accumulator_2;
        limb_accumulator_identity *= q_3;

        /**
         * MEMORY
         *
         * A RAM memory record contains a tuple of the following fields:
         *  * i: `index` of memory cell being accessed
         *  * t: `timestamp` of memory cell being accessed (used for RAM, set to 0 for ROM)
         *  * v: `value` of memory cell being accessed
         *  * a: `access` type of record. read: 0 = read, 1 = write
         *  * r: `record` of memory cell. record = access + index * eta + timestamp * eta^2 + value * eta^3
         *
         * A ROM memory record contains a tuple of the following fields:
         *  * i: `index` of memory cell being accessed
         *  * v: `value1` of memory cell being accessed (ROM tables can store up to 2 values per index)
         *  * v2:`value2` of memory cell being accessed (ROM tables can store up to 2 values per index)
         *  * r: `record` of memory cell. record = index * eta + value2 * eta^2 + value1 * eta^3
         *
         *  When performing a read/write access, the values of i, t, v, v2, a, r are stored in the following wires +
         * selectors, depending on whether the gate is a RAM read/write or a ROM read
         *
         *  | gate type | i  | v2/t  |  v | a  | r  |
         *  | --------- | -- | ----- | -- | -- | -- |
         *  | ROM       | w1 | w2    | w3 | -- | w4 |
         *  | RAM       | w1 | w2    | w3 | qc | w4 |
         *
         * (for accesses where `index` is a circuit constant, it is assumed the circuit will apply a copy constraint on
         * `w2` to fix its value)
         *
         **/

        /**
         * Memory Record Check
         *
         * A ROM/ROM access gate can be evaluated with the identity:
         *
         * qc + w1 \eta + w2 \eta^2 + w3 \eta^3 - w4 = 0
         *
         * For ROM gates, qc = 0
         */
        auto memory_record_check = w_3 * eta;
        memory_record_check += w_2;
        memory_record_check *= eta;
        memory_record_check += w_1;
        memory_record_check *= eta;
        memory_record_check += q_c;
        auto partial_record_check = memory_record_check; // used in RAM consistency check
        memory_record_check = memory_record_check - w_4;

        /**
         * ROM Consistency Check
         *
         * For every ROM read, a set equivalence check is applied between the record witnesses, and a second set of
         * records that are sorted.
         *
         * We apply the following checks for the sorted records:
         *
         * 1. w1, w2, w3 correctly map to 'index', 'v1, 'v2' for a given record value at w4
         * 2. index values for adjacent records are monotonically increasing
         * 3. if, at gate i, index_i == index_{i + 1}, then value1_i == value1_{i + 1} and value2_i == value2_{i + 1}
         *
         */
        auto index_delta = w_1_shift - w_1;
        auto record_delta = w_4_shift - w_4;

        auto index_is_monotonically_increasing = index_delta * index_delta - index_delta;

        auto adjacent_values_match_if_adjacent_indices_match = (index_delta * FF(-1) + FF(1)) * record_delta;

        std::get<1>(accumulators) +=
            adjacent_values_match_if_adjacent_indices_match * (q_1 * q_2) * (q_aux * scaling_factor);
        std::get<2>(accumulators) += index_is_monotonically_increasing * (q_1 * q_2) * (q_aux * scaling_factor);
        auto ROM_consistency_check_identity = memory_record_check * (q_1 * q_2);

        /**
         * RAM Consistency Check
         *
         * The 'access' type of the record is extracted with the expression `w_4 - partial_record_check`
         * (i.e. for an honest Prover `w1 * eta + w2 * eta^2 + w3 * eta^3 - w4 = access`.
         * This is validated by requiring `access` to be boolean
         *
         * For two adjacent entries in the sorted list if _both_
         *  A) index values match
         *  B) adjacent access value is 0 (i.e. next gate is a READ)
         * then
         *  C) both values must match.
         * The gate boolean check is
         * (A && B) => C  === !(A && B) || C ===  !A || !B || C
         *
         * N.B. it is the responsibility of the circuit writer to ensure that every RAM cell is initialized
         * with a WRITE operation.
         */
        auto access_type = (w_4 - partial_record_check);             // will be 0 or 1 for honest Prover
        auto access_check = access_type * access_type - access_type; // check value is 0 or 1

        // TODO: oof nasty compute here. If we sorted in reverse order we could re-use `partial_record_check`
        auto next_gate_access_type = w_3_shift * eta;
        next_gate_access_type += w_2_shift;
        next_gate_access_type *= eta;
        next_gate_access_type += w_1_shift;
        next_gate_access_type *= eta;
        next_gate_access_type = w_4_shift - next_gate_access_type;

        auto value_delta = w_3_shift - w_3;
        auto adjacent_values_match_if_adjacent_indices_match_and_next_access_is_a_read_operation =
            (index_delta * FF(-1) + FF(1)) * value_delta * (next_gate_access_type * FF(-1) + FF(1));

        // We can't apply the RAM consistency check identity on the final entry in the sorted list (the wires in the
        // next gate would make the identity fail).
        // We need to validate that its 'access type' bool is correct. Can't do
        // with an arithmetic gate because of the `eta` factors. We need to check that the *next* gate's access type is
        // correct, to cover this edge case
        auto next_gate_access_type_is_boolean = next_gate_access_type * next_gate_access_type - next_gate_access_type;

        // Putting it all together...
        std::get<3>(accumulators) +=
            adjacent_values_match_if_adjacent_indices_match_and_next_access_is_a_read_operation * (q_arith) *
            (q_aux * scaling_factor);
        std::get<4>(accumulators) += index_is_monotonically_increasing * (q_arith) * (q_aux * scaling_factor);
        std::get<5>(accumulators) += next_gate_access_type_is_boolean * (q_arith) * (q_aux * scaling_factor);
        auto RAM_consistency_check_identity = access_check * (q_arith);

        /**
         * RAM Timestamp Consistency Check
         *
         * | w1 | w2 | w3 | w4 |
         * | index | timestamp | timestamp_check | -- |
         *
         * Let delta_index = index_{i + 1} - index_{i}
         *
         * Iff delta_index == 0, timestamp_check = timestamp_{i + 1} - timestamp_i
         * Else timestamp_check = 0
         */
        auto timestamp_delta = w_2_shift - w_2;
        auto RAM_timestamp_check_identity = (index_delta * FF(-1) + FF(1)) * timestamp_delta - w_3;

        /**
         * The complete RAM/ROM memory identity
         *
         */
        auto memory_identity = ROM_consistency_check_identity;
        memory_identity += RAM_timestamp_check_identity * (q_4 * q_1);
        memory_identity += memory_record_check * (q_m * q_1);
        memory_identity += RAM_consistency_check_identity;

        auto auxiliary_identity = memory_identity + non_native_field_identity + limb_accumulator_identity;
        auxiliary_identity *= (q_aux * scaling_factor);
        std::get<0>(accumulators) += auxiliary_identity;
    };
};

template <typename FF> using AuxiliaryRelation = RelationWrapper<FF, AuxiliaryRelationBase>;
} // namespace proof_system::honk::sumcheck
