/**
 * @ Author: Pierre Veysseyre
 * @ Description: ParameterTable.cpp
 */

#include "ParameterTable.hpp"

using namespace Audio;

std::size_t ParameterTable::_instancesCount = 0u;
Core::FlatVector<std::size_t> ParameterTable::_instancesUsed;
