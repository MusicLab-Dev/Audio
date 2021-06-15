/**
 * @ Author: Pierre Veysseyre
 * @ Description: FM algorithm generator
 */

#pragma once

#include <array>

namespace Audio::DSP::FM::Generator
{
    struct ColumnData
    {
        std::array<Node *, 7> children {};
        std::size_t count { 0u };
    };

    struct GroupData
    {
        std::array<Node *, 7> children {};
        std::size_t count { 0u };
    };

    struct OperatorData
    {
        float frequencyDelta { 0.0f };
        float attack { 0.0f };
        float decay { 0.0f };
        float sustain { 0.0f };
        float release { 0.0f };
        float volume { 0.0f };
    };

    struct OperatorFeedbackData : public OperatorData
    {
        Node *feedbackTarget { nullptr };
        std::size_t feedbackCount { 0u };
    };

    struct Node
    {
        union Data {
            ColumnData columnData {};
            GroupData groupData;
            OperatorData operatorData;
            OperatorFeedbackData operatorFeedbackData;
        };

        enum class Type : std::uint32_t {
            Column = 0,
            Group,
            Operator,
            OperatorFeedback
        };


        Type type { Type::Column };
        Node *previous { nullptr };
        Node *next { nullptr };
        Data data {};
    };

    struct Tree
    {
        std::size_t count { 0u };
        std::array<Node, 32> nodes {};
    };

#define GENERATE_FM_ALGORITHM(Name, ...)
#define REGISTER_COLUMN(...)
#define REGISTER_GROUP(...)
#define REGISTER_OPERATOR(...)
#define REGISTER_OPERATOR_FEEDBACK(...)

        // [
        //     { 1, 2, 3 },
        //     { 4, { 5, 6 } }
        // ],

    // GENERATE_FM_ALGORITHM(MyAlgorithm,
    //     REGISTER_FM_COLUMN()
    // )

    constexpr auto MyAlgorithm = []() -> Tree {
        Tree tree;
        tree.count = 4;
        return tree;
    }();

    static_assert(MyAlgorithm.count == 4);

    // template<Tree AlgorithmTree>
    // [[nodiscard]] constexpr std::size_t Process(void)
    // {
    //     if constexpr (AlgorithmTree->nodes[0].type == Node::Type::Column) {
    //         return 1u;
    //     } else if constexpr (AlgorithmTree->nodes[0].type == Node::Type::Group) {
    //         return 2u;
    //     } else if constexpr (AlgorithmTree->nodes[0].type == Node::Type::Operator) {
    //         return 3u;
    //     } else if constexpr (AlgorithmTree->nodes[0].type == Node::Type::OperatorFeedback) {
    //         return 4u;
    //     }
    // }

    // static_assert(ProcessMyAlgorithm() == 1u);
}
