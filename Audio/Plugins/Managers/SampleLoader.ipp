/**
 * @ Author: Pierre Veysseyre
 * @ Description: SampleLoader.cpp
 */

#include <filesystem>
#include <fstream>

#include <iostream>

template<typename T>
inline Audio::Buffer Audio::SampleLoader::LoadWAV(const std::string &path)
{
    Specs_WAV specs;
    auto buffer = LoadWAV(path, specs);

    // Convert buffer into template type according specs

    return buffer;
}