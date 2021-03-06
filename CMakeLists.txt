cmake_minimum_required(VERSION 3.13 FATAL_ERROR)

get_filename_component(AudioRoot ${CMAKE_CURRENT_LIST_FILE} PATH)

include(${AudioRoot}/CompileOptions/ToolchainOptions.cmake)

project(Audio)

include(${AudioRoot}/CompileOptions/CompileOptions.cmake)

add_submodule_directory(${AudioRoot}/Flow)

include(${AudioRoot}/Audio/Audio.cmake)

if(TESTS)
    enable_testing()
    include(${AudioRoot}/Tests/AudioTests.cmake)
endif()

if(BENCHMARKS)
    include(${AudioRoot}/Benchmarks/AudioBenchmarks.cmake)
endif()

include(${AudioRoot}/Tools/Tools.cmake)
