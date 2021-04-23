cmake_minimum_required(VERSION 3.10 FATAL_ERROR)
project(Audio)

find_package(SDL2 REQUIRED)

get_filename_component(AudioDir ${CMAKE_CURRENT_LIST_FILE} PATH)

set(AudioSources
    ${AudioDir}/AScheduler.hpp
    ${AudioDir}/AScheduler.ipp
    ${AudioDir}/AScheduler.cpp
    ${AudioDir}/SchedulerTask.hpp
    ${AudioDir}/SchedulerTask.ipp
    ${AudioDir}/Automation.hpp
    ${AudioDir}/Base.hpp
    ${AudioDir}/BaseVolume.hpp
    ${AudioDir}/BaseDevice.hpp
    ${AudioDir}/BaseIndex.hpp
    ${AudioDir}/BaseIndex.cpp
    ${AudioDir}/Math.hpp
    ${AudioDir}/Buffer.hpp
    ${AudioDir}/BufferOctave.hpp
    ${AudioDir}/Buffer.ipp
    ${AudioDir}/Buffer.cpp
    ${AudioDir}/Connection.hpp
    ${AudioDir}/Control.hpp
    ${AudioDir}/Controls.hpp
    ${AudioDir}/ParameterTable.hpp
    ${AudioDir}/ParameterTable.cpp
    ${AudioDir}/Device.cpp
    ${AudioDir}/Device.hpp
    ${AudioDir}/ExternalFactory.hpp
    ${AudioDir}/FlatNode.hpp
    ${AudioDir}/FlatNote.hpp
    ${AudioDir}/InternalFactory.hpp
    ${AudioDir}/IPlugin.hpp
    ${AudioDir}/IPluginFactory.hpp
    ${AudioDir}/Node.hpp
    ${AudioDir}/Node.ipp
    ${AudioDir}/Note.hpp
    ${AudioDir}/Partitions.hpp
    ${AudioDir}/Note.cpp
    ${AudioDir}/Note.ipp
    ${AudioDir}/Partition.hpp
    ${AudioDir}/PluginPtr.hpp
    ${AudioDir}/PluginPtr.ipp
    ${AudioDir}/PluginTable.cpp
    ${AudioDir}/PluginTable.hpp
    ${AudioDir}/PluginTable.ipp
    ${AudioDir}/PluginUtils.hpp
    ${AudioDir}/PluginControlUtils.hpp
    ${AudioDir}/Project.hpp
    ${AudioDir}/Project.ipp
    ${AudioDir}/UtilsMidi.hpp
    ${AudioDir}/KissFFT.hpp
    ${AudioDir}/KissFFT.ipp
    ${AudioDir}/Volume.hpp
    ${AudioDir}/Volume.ipp
)


set(AudioPluginsDir ${AudioDir}/Plugins)

set(AudioPluginsSources
    ${AudioPluginsDir}/Sampler.hpp
    ${AudioPluginsDir}/Sampler.ipp
    ${AudioPluginsDir}/Oscillator.hpp
    ${AudioPluginsDir}/Oscillator.ipp
    ${AudioPluginsDir}/Mixer.hpp
    ${AudioPluginsDir}/Mixer.ipp
    ${AudioPluginsDir}/LambdaFilter.hpp
    ${AudioPluginsDir}/LambdaFilter.ipp
)

set(AudioPluginManagerDir ${AudioPluginsDir}/Managers)

set(AudioPluginManagersSources
    ${AudioPluginManagerDir}/NoteManager.hpp
    ${AudioPluginManagerDir}/NoteManager.ipp
)

set(AudioSampleFileDir ${AudioDir}/SampleFile)

set(AudioPluginManagersSources
    ${AudioSampleFileDir}/SampleManager.hpp
    ${AudioSampleFileDir}/SampleManager.ipp
    ${AudioSampleFileDir}/SampleManager.cpp
    ${AudioSampleFileDir}/SampleManagerSpecs.hpp
    ${AudioSampleFileDir}/SampleManagerWAV.hpp
    ${AudioSampleFileDir}/SampleManagerWAV.cpp
    ${AudioSampleFileDir}/SampleManagerWAV.ipp
)


set(AudioDSPDir ${AudioDir}/DSP)

set(AudioDSPSources
    ${AudioDSPDir}/Merge.hpp
    ${AudioDSPDir}/Merge.ipp
    ${AudioDSPDir}/PitchShift.hpp
    ${AudioDSPDir}/PitchShift.cpp
    ${AudioDSPDir}/PitchShift.ipp
    ${AudioDSPDir}/Resampler.hpp
    ${AudioDSPDir}/Resampler.ipp
    ${AudioDSPDir}/Interpolation.ipp
    ${AudioDSPDir}/Decimation.ipp
    ${AudioDSPDir}/Biquad.hpp
    ${AudioDSPDir}/Biquad.ipp
    ${AudioDSPDir}/Biquad.cpp
    ${AudioDSPDir}/EnveloppeGenerator.hpp
    ${AudioDSPDir}/EnveloppeGenerator.ipp
    ${AudioDSPDir}/FIR.hpp
    ${AudioDSPDir}/FIR.ipp
    ${AudioDSPDir}/Filter.hpp
    ${AudioDSPDir}/Filter.ipp
    ${AudioDSPDir}/Window.ipp
)

set(AudioKissFFTDir ${PROJECT_SOURCE_DIR}/kissfft)

set(AudioKissFFTSources
    ${AudioKissFFTDir}/kiss_fft.h
    ${AudioKissFFTDir}/kiss_fft.c
    ${AudioKissFFTDir}/tools/kiss_fftr.h
    ${AudioKissFFTDir}/tools/kiss_fftr.c
)

add_library(${PROJECT_NAME}
    ${AudioSources}
    ${AudioPluginsSources}
    ${AudioPluginManagersSources}
    ${AudioDSPSources}
    ${AudioKissFFTSources}
)

target_include_directories(${PROJECT_NAME} PUBLIC ${AudioDir}/..)
target_include_directories(${PROJECT_NAME} PUBLIC ${AudioDir}/../kissfft/)

target_link_libraries(${PROJECT_NAME} PUBLIC Core Flow)

if(MSVC)
    target_link_libraries(${PROJECT_NAME} PUBLIC SDL2::SDL2)
else()
    target_link_libraries(${PROJECT_NAME} PUBLIC SDL2)
endif()

if(CODE_COVERAGE)
    target_compile_options(${PROJECT_NAME} PUBLIC --coverage)
    target_link_options(${PROJECT_NAME} PUBLIC --coverage)
endif()