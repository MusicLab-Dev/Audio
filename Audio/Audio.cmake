cmake_minimum_required(VERSION 3.10 FATAL_ERROR)
project(Audio)

find_package(SDL2 REQUIRED)

get_filename_component(AudioDir ${CMAKE_CURRENT_LIST_FILE} PATH)

set(AudioPrecompiledHeaders
    ${AudioDir}/AScheduler.hpp
    ${AudioDir}/SchedulerTask.hpp
    ${AudioDir}/Automation.hpp
    ${AudioDir}/Base.hpp
    ${AudioDir}/BaseVolume.hpp
    ${AudioDir}/BaseDevice.hpp
    ${AudioDir}/BaseIndex.hpp
    ${AudioDir}/Math.hpp
    ${AudioDir}/Buffer.hpp
    ${AudioDir}/Modifier.hpp
    ${AudioDir}/BufferOctave.hpp
    ${AudioDir}/Connection.hpp
    ${AudioDir}/ControlEvent.hpp
    ${AudioDir}/ParameterTable.hpp
    ${AudioDir}/Device.hpp
    ${AudioDir}/ExternalFactory.hpp
    ${AudioDir}/FlatNode.hpp
    ${AudioDir}/FlatNote.hpp
    ${AudioDir}/InternalFactory.hpp
    ${AudioDir}/IPlugin.hpp
    ${AudioDir}/IPluginFactory.hpp
    ${AudioDir}/Node.hpp
    ${AudioDir}/Note.hpp
    ${AudioDir}/Partitions.hpp
    ${AudioDir}/Partition.hpp
    ${AudioDir}/PluginPtr.hpp
    ${AudioDir}/PluginTable.hpp
    ${AudioDir}/PluginUtils.hpp
    ${AudioDir}/Project.hpp
    ${AudioDir}/UtilsMidi.hpp
    ${AudioDir}/Volume.hpp
)

set(AudioSources
    ${AudioPrecompiledHeaders}
    ${AudioDir}/PluginUtilsControls.hpp
    ${AudioDir}/PluginUtilsControlsEnvelope.hpp
    ${AudioDir}/PluginUtilsControlsEqualizer.hpp
    ${AudioDir}/PluginUtilsControlsFilter.hpp
    ${AudioDir}/PluginUtilsControlsFM.hpp
    ${AudioDir}/PluginUtilsControlsVolume.hpp
    ${AudioDir}/AScheduler.ipp
    ${AudioDir}/AScheduler.cpp
    ${AudioDir}/SchedulerTask.ipp
    ${AudioDir}/BaseIndex.cpp
    ${AudioDir}/Buffer.ipp
    ${AudioDir}/Buffer.cpp
    ${AudioDir}/ParameterTable.cpp
    ${AudioDir}/Device.cpp
    ${AudioDir}/Node.ipp
    ${AudioDir}/Note.cpp
    ${AudioDir}/Note.ipp
    ${AudioDir}/PluginPtr.ipp
    ${AudioDir}/PluginTable.cpp
    ${AudioDir}/PluginTable.ipp
    ${AudioDir}/Project.ipp
    ${AudioDir}/Volume.ipp
)


set(AudioPluginsDir ${AudioDir}/Plugins)

set(AudioPluginsSources
    ${AudioPluginsDir}/Sampler.hpp
    ${AudioPluginsDir}/Sampler.ipp
    ${AudioPluginsDir}/Oscillator.hpp
    ${AudioPluginsDir}/Oscillator.ipp
    ${AudioPluginsDir}/FMSynth.hpp
    ${AudioPluginsDir}/FMSynth.ipp
    ${AudioPluginsDir}/Mixer.hpp
    ${AudioPluginsDir}/Mixer.ipp
    ${AudioPluginsDir}/LambdaFilter.hpp
    ${AudioPluginsDir}/LambdaFilter.ipp
    ${AudioPluginsDir}/BasicFilter.hpp
    ${AudioPluginsDir}/BasicFilter.ipp
    ${AudioPluginsDir}/BandFilter.hpp
    ${AudioPluginsDir}/BandFilter.ipp
    ${AudioPluginsDir}/SigmaFilter.hpp
    ${AudioPluginsDir}/SigmaFilter.ipp
    ${AudioPluginsDir}/GammaEqualizer.hpp
    ${AudioPluginsDir}/GammaEqualizer.ipp
    ${AudioPluginsDir}/SimpleDelay.hpp
    ${AudioPluginsDir}/SimpleDelay.ipp
    ${AudioPluginsDir}/Arpeggiator.hpp
    ${AudioPluginsDir}/Arpeggiator.ipp
    ${AudioPluginsDir}/Chords.hpp
    ${AudioPluginsDir}/Chords.ipp
)

set(AudioPluginManagerDir ${AudioPluginsDir}/Managers)

set(AudioSampleFileDir ${AudioDir}/SampleFile)

set(AudioPluginManagersSources
    ${AudioPluginManagerDir}/NoteManager.hpp
    ${AudioPluginManagerDir}/NoteManager.ipp
    ${AudioPluginManagerDir}/FMManager.hpp
    ${AudioPluginManagerDir}/FMManager.ipp
    ${AudioPluginManagerDir}/ArpeggiatorManager.hpp
    ${AudioPluginManagerDir}/ArpeggiatorManager.ipp
    ${AudioPluginManagerDir}/ChordsManager.hpp
    ${AudioPluginManagerDir}/ChordsManager.ipp
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
    ${AudioDSPDir}/Converter.hpp
    ${AudioDSPDir}/Converter.ipp
    ${AudioDSPDir}/Reformater.hpp
    ${AudioDSPDir}/Reformater.ipp
    ${AudioDSPDir}/Resampler.hpp
    ${AudioDSPDir}/Resampler.ipp
    ${AudioDSPDir}/Interpolation.ipp
    ${AudioDSPDir}/Decimation.ipp
    ${AudioDSPDir}/Biquad.hpp
    ${AudioDSPDir}/Biquad.ipp
    ${AudioDSPDir}/Biquad.cpp
    ${AudioDSPDir}/EnvelopeGenerator.hpp
    ${AudioDSPDir}/EnvelopeGenerator.ipp
    ${AudioDSPDir}/FIR.hpp
    ${AudioDSPDir}/FM.hpp
    ${AudioDSPDir}/FMGenerator.hpp
    ${AudioDSPDir}/FMGenerator.cpp
    ${AudioDSPDir}/FM.ipp
    ${AudioDSPDir}/FIR.ipp
    ${AudioDSPDir}/FIRFilter.ipp
    ${AudioDSPDir}/Filter.hpp
    ${AudioDSPDir}/Filter.ipp
    ${AudioDSPDir}/Window.ipp
    ${AudioDSPDir}/Filter.hpp
    ${AudioDSPDir}/Delay.hpp
    ${AudioDSPDir}/Delay.ipp
)

add_library(${PROJECT_NAME}
    ${AudioSources}
    ${AudioPluginsSources}
    ${AudioPluginManagersSources}
    ${AudioDSPSources}
)

target_precompile_headers(${PROJECT_NAME} PUBLIC ${AudioPrecompiledHeaders})

target_include_directories(${PROJECT_NAME} PUBLIC ${AudioDir}/..)

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
