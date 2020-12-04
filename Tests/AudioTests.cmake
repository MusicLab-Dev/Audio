project(AudioTests)

find_package(GTest REQUIRED)

get_filename_component(AudioTestsDir ${CMAKE_CURRENT_LIST_FILE} PATH)

set(AudioTestsSources
    ${AudioTestsDir}/tests_Automation.cpp
    # ${AudioTestsDir}/tests_AScheduler.cpp
    # ${AudioTestsDir}/tests_SchedulerTask.cpp
    ${AudioTestsDir}/tests_Buffer.cpp
    ${AudioTestsDir}/tests_Control.cpp
    ${AudioTestsDir}/tests_Device.cpp
    ${AudioTestsDir}/tests_IPlugin.cpp
    ${AudioTestsDir}/tests_Note.cpp
    ${AudioTestsDir}/tests_NoteManager.cpp
    ${AudioTestsDir}/tests_Partition.cpp
    ${AudioTestsDir}/tests_PluginTable.cpp
    ${AudioTestsDir}/tests_Point.cpp
    ${AudioTestsDir}/tests_Project.cpp
    ${AudioTestsDir}/tests_UtilsMidi.cpp

    # ${AudioTestsDir}/tests_Biquad.cpp
    # ${AudioTestsDir}/tests_Merge.cpp
    ${AudioTestsDir}/tests_EnveloppeGenerator.cpp
)

add_executable(${PROJECT_NAME} ${AudioTestsSources})

add_test(NAME ${PROJECT_NAME} COMMAND ${PROJECT_NAME})

target_link_libraries(${PROJECT_NAME}
PUBLIC
    Audio
    GTest::GTest GTest::Main
)