/**
 * @ Author: Pierre Veysseyre
 * @ Description: Unit tests of Partition decoer
 */

#include <gtest/gtest.h>
#include <iostream>

#include <PartitionDecoder.hpp>

using namespace Tools;

TEST(PartitionDecoder, InputFile)
{
    rapidjson::Document doc;
    JsonParser::Parse("/home/Pedro/_tekno4/Audio/Tools/partition.part", doc);

    std::cout << doc.HasMember("channels") << std::endl;

}
