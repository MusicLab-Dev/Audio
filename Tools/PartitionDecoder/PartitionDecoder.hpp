/**
 * @ Author: Pierre Veysseyre
 * @ Description: PartitionDecoder.hpp
 */

#pragma once

#include <rapidjson/document.h>
#include <rapidjson/istreamwrapper.h>
#include <fstream>

#include <iostream>

namespace Tools
{
    struct JsonParser;
}

struct Tools::JsonParser
{
    static void Parse(const std::string &path, rapidjson::Document &doc) {
        // rapidjson::Document doc;
        std::ifstream fileStream(path);
        rapidjson::IStreamWrapper streamWrapper(fileStream);

        doc.ParseStream(streamWrapper);
        std::cout << doc.GetParseError() << std::endl;

    }
};