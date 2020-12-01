cmake_minimum_required(VERSION 3.10 FATAL_ERROR)
project(Audio)

find_package(SDL2 REQUIRED)

get_filename_component(AudioDir ${CMAKE_CURRENT_LIST_FILE} PATH)

set(AudioSources
    ${AudioDir}/AScheduler.hpp
    ${AudioDir}/AScheduler.ipp
    ${AudioDir}/AScheduler.cpp
    ${AudioDir}/TaskBase.hpp
    ${AudioDir}/TaskAudio.hpp
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
    ${AudioDir}/Project.hpp
    ${AudioDir}/Project.ipp
    ${AudioDir}/UtilsMidi.hpp
)


set(AudioPluginsDir ${AudioDir}/Plugins)

set(AudioPluginsSources
    ${AudioPluginsDir}/Oscillator.hpp
    ${AudioPluginsDir}/Oscillator.ipp
    ${AudioPluginsDir}/SimpleDelay.hpp
    ${AudioPluginsDir}/SimpleDelay.ipp
    ${AudioPluginsDir}/Mixer.hpp
    ${AudioPluginsDir}/Mixer.ipp
)


set(AudioDSPDir ${AudioDir}/DSP)

set(AudioDSPSources
    ${AudioDSPDir}/Biquad.cpp
    ${AudioDSPDir}/Biquad.hpp
    ${AudioDSPDir}/Biquad.ipp
    ${AudioDSPDir}/EnveloppeGenerator.hpp
    ${AudioDSPDir}/EnveloppeGenerator.ipp
)

add_library(${PROJECT_NAME}
    ${AudioSources}
    ${AudioPluginsSources}
    ${AudioDSPSources}
)

target_include_directories(${PROJECT_NAME} PUBLIC ${AudioDir}/..)

target_link_libraries(${PROJECT_NAME} PUBLIC Core Taskflow)

if(MSVC)
    target_link_libraries(${PROJECT_NAME} PUBLIC SDL2::SDL2)
else()
    target_link_libraries(${PROJECT_NAME} PUBLIC SDL2)
endif()

if(CODE_COVERAGE)
    target_compile_options(${PROJECT_NAME} PUBLIC --coverage)
    target_link_options(${PROJECT_NAME} PUBLIC --coverage)
endif()