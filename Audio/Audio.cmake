cmake_minimum_required(VERSION 3.10 FATAL_ERROR)
project(Audio)

find_package(SDL2 REQUIRED)

if(WIN32)
    add_library(SDL2Target INTERFACE)
    target_link_libraries(SDL2::SDL2)
else()
    add_library(SDL2Target INTERFACE)
    target_link_libraries(SDL2)
endif()

get_filename_component(AudioDir ${CMAKE_CURRENT_LIST_FILE} PATH)

set(AudioSources
    ${AudioDir}/AScheduler.hpp
    ${AudioDir}/Automation.hpp
    ${AudioDir}/Base.hpp
    ${AudioDir}/BaseDevice.hpp
    ${AudioDir}/BaseIndex.hpp
    ${AudioDir}/Buffer.hpp
    ${AudioDir}/Connection.hpp
    ${AudioDir}/Control.hpp
    ${AudioDir}/Control.ipp
    ${AudioDir}/Device.cpp
    ${AudioDir}/Device.hpp
    ${AudioDir}/Device.ipp
    ${AudioDir}/ExternalFactory.hpp
    ${AudioDir}/FlatNode.hpp
    ${AudioDir}/FlatNote.hpp
    ${AudioDir}/InternalFactory.hpp
    ${AudioDir}/IPlugin.hpp
    ${AudioDir}/IPluginFactory.hpp
    ${AudioDir}/Node.hpp
    ${AudioDir}/Node.ipp
    ${AudioDir}/Note.hpp
    ${AudioDir}/Note.ipp
    ${AudioDir}/Partition.hpp
    ${AudioDir}/Partition.ipp
    ${AudioDir}/PluginPtr.hpp
    ${AudioDir}/PluginPtr.ipp
    ${AudioDir}/PluginTable.cpp
    ${AudioDir}/PluginTable.hpp
    ${AudioDir}/PluginTable.ipp
    ${AudioDir}/Project.cpp
    ${AudioDir}/Project.hpp
    ${AudioDir}/UtilsMidi.hpp
)

set(AudioDSPDir ${AudioDir}/DSP)

set(AudioDSPSources
    # ${AudioDSPDir}/Biquad.cpp
    # ${AudioDSPDir}/Biquad.hpp
    # ${AudioDSPDir}/Biquad.ipp
    # ${AudioDSPDir}/EnveloppeGenerator.hpp
    # ${AudioDSPDir}/EnveloppeGenerator.ipp
)

add_library(${PROJECT_NAME} ${AudioSources} ${AudioDSPSources})

target_include_directories(${PROJECT_NAME} PUBLIC ${AudioDir}/..)

target_link_libraries(${PROJECT_NAME} PUBLIC Core SDL2Target)

if(CODE_COVERAGE)
    target_compile_options(${PROJECT_NAME} PUBLIC --coverage)
    target_link_options(${PROJECT_NAME} PUBLIC --coverage)
endif()