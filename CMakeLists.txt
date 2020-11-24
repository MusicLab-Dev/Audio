cmake_minimum_required(VERSION 3.13 FATAL_ERROR)
project(Audio)

get_filename_component(AudioRoot ${CMAKE_CURRENT_LIST_FILE} PATH)

# Include taskflow
option(TF_BUILD_TESTS OFF)
option(TF_BUILD_EXAMPLES OFF)
add_subdirectory(${AudioRoot}/External/taskflow)

include(${AudioRoot}/CompileOptions/CompileOptions.cmake)

add_submodule_directory(${AudioRoot}/Core)

include(${AudioRoot}/Audio/Audio.cmake)

if(TESTS)
    enable_testing()
    include(${AudioRoot}/Tests/AudioTests.cmake)
endif()

if(BENCHMARKS)
    include(${AudioRoot}/Benchmarks/AudioBenchmarks.cmake)
endif()