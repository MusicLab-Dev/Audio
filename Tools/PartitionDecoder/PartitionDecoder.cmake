project(PartitionDecoder)

get_filename_component(PartitionDecoderDir ${CMAKE_CURRENT_LIST_FILE} PATH)

set(JsonLibraryDirRoot ${CMAKE_CURRENT_SOURCE_DIR}/rapidjson)
set(JsonLibrarySourcesDir ${JsonLibraryDirRoot}/include/rapidjson)

set(JsonLibrarySources
    ${JsonLibrarySourcesDir}/document.h
    ${JsonLibrarySourcesDir}/reader.h
)

set(PartitionDecoderSources
    ${PartitionDecoderDir}/PartitionDecoder.hpp
    ${PartitionDecoderDir}/PartitionDecoder.cpp
)

add_library(${PROJECT_NAME}
    ${JsonLibrarySources}
    ${PartitionDecoderSources}
)

target_include_directories(${PROJECT_NAME}
PUBLIC
    ${JsonLibraryDirRoot}/include
)

if(TESTS)
    enable_testing()
    include(${PartitionDecoderDir}/Tests/PartitionDecoderTests.cmake)
endif()
