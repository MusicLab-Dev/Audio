/**
 * @ Author: Pierre Veysseyre
 * @ Description: Unit tests of ParameterTable
 */

#include <gtest/gtest.h>

#include <Audio/ParameterTable.hpp>

using namespace Audio;

TEST(ParameterTable, BasicIndexGeneration)
{
    // ParamID index0 = ParameterTable::GenerateIdNewInstance();
    // EXPECT_EQ(index0, 0);

    // for (auto i = 1u; i < ParameterTable::MaxInstances; ++i) {
    //     ParamID idx = ParameterTable::GenerateIdNewInstance();
    //     EXPECT_EQ(idx, ParameterTable::MaxParameterPerInstance * i);
    // }
//     // EXPECT_THROW(auto idx = ParameterTable::GenerateIdNewInstance(), std::runtime_error);
}

TEST(ParameterTable, RemoveInstance)
{
    EXPECT_FALSE(ParameterTable::RemoveInstance(0));

    ParamID index = ParameterTable::GenerateIdNewInstance();
    EXPECT_EQ(index, 0);
    EXPECT_TRUE(ParameterTable::RemoveInstance(0));
}