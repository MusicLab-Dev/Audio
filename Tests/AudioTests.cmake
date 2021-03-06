project(AudioTests)

find_package(GTest REQUIRED)

get_filename_component(AudioTestsDir ${CMAKE_CURRENT_LIST_FILE} PATH)

set(AudioTestsSources
    ${AudioTestsDir}/tests_BeatRange.cpp
    ${AudioTestsDir}/tests_Buffer.cpp
    ${AudioTestsDir}/tests_Point.cpp
    ${AudioTestsDir}/tests_Automation.cpp
    ${AudioTestsDir}/tests_Automations.cpp
    ${AudioTestsDir}/tests_ParameterTable.cpp
    ${AudioTestsDir}/tests_PluginTable.cpp
    ${AudioTestsDir}/tests_Sampler.cpp
    ${AudioTestsDir}/tests_Partition.cpp
    ${AudioTestsDir}/tests_NoteManager.cpp
    ${AudioTestsDir}/tests_Merge.cpp
    ${AudioTestsDir}/tests_Resampler.cpp
    ${AudioTestsDir}/tests_Biquad.cpp
    ${AudioTestsDir}/tests_EnvelopeGenerator.cpp
    ${AudioTestsDir}/tests_Project.cpp

    ${AudioTestsDir}/tests_Reformater.cpp

    # ${AudioTestsDir}/tests_SchedulerTask.cpp
    # ${AudioTestsDir}/tests_Device.cpp
    # ${AudioTestsDir}/tests_UtilsMidi.cpp
)

add_executable(${PROJECT_NAME} ${AudioTestsSources})

add_test(NAME ${PROJECT_NAME} COMMAND ${PROJECT_NAME})

target_link_libraries(${PROJECT_NAME}
PUBLIC
    Audio
    GTest::GTest GTest::Main
)
