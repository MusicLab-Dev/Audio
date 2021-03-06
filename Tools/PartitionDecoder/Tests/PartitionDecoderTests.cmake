project(PartitionDecoderTests)

find_package(GTest REQUIRED)

get_filename_component(PartitionDecoderTestsDir ${CMAKE_CURRENT_LIST_FILE} PATH)

set(PartitionDecoderTestsSources
    ${PartitionDecoderTestsDir}/tests_PartitionDecoder.cpp
)

add_executable(${PROJECT_NAME} ${PartitionDecoderTestsSources})

add_test(NAME ${PROJECT_NAME} COMMAND ${PROJECT_NAME})

target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC ${PartitionDecoderDir})

target_link_libraries(${PROJECT_NAME}
PUBLIC
    PartitionDecoder
    GTest::GTest GTest::Main
)