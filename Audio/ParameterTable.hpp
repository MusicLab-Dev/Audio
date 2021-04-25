/**
 * @ Author: Pierre Veysseyre
 * @ Description: ParameterTable.hpp
 */

#pragma once

#include "Base.hpp"

namespace Audio
{
    struct ParameterTable;
}

#include <iostream>

struct Audio::ParameterTable
{
    /** @brief Max parameters allowed per instances (node) */
    static constexpr std::size_t MaxParameterPerInstance = 128u;
    /** @brief Max instances deduced */
    static constexpr auto MaxInstances = (std::numeric_limits<ParamID>::max() + 1ull) / MaxParameterPerInstance;

    // static std::size_t Intace


    /** @brief Generate a new parameter index for a new instance */
    [[nodiscard]] static ParamID GenerateIdNewInstance(void) {
        std::size_t iInstance { 0u };
        // std::cout << "======\n";
        if (_instancesUsed.size() == MaxInstances)
            throw std::runtime_error("Audio::ParameterTable::GenerateIdNewInstance: Maximum instances reached.");

        for (iInstance = 0u; iInstance < MaxInstances; ++iInstance) {
            if (auto it = std::find(_instancesUsed.begin(), _instancesUsed.end(), iInstance * MaxParameterPerInstance); it == _instancesUsed.end()) {
                _instancesUsed.push(iInstance * MaxParameterPerInstance);
                _instancesCount++;
                return static_cast<ParamID>(iInstance * MaxParameterPerInstance);
            }
        }
        throw std::runtime_error("Audio::ParameterTable::GenerateIdNewInstance: Maximum instances reached.");
    }

    /** @brief Generate a new parameter index for an existing instance (the last parameter index for this instance is used) */
    [[nodiscard]] static ParamID GenerateId(const ParamID paramId) noexcept {
        return paramId + 1;
    }

    static bool RemoveInstance(const ParamID sourceId) noexcept {
        if (auto it = std::find(_instancesUsed.begin(), _instancesUsed.end(), sourceId); it == _instancesUsed.end())
            return false;

        return true;
    }

private:
    static std::size_t _instancesCount;
    static Core::FlatVector<std::size_t> _instancesUsed;

};
