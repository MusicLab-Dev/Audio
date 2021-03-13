project(AudioTools)

get_filename_component(ToolsDir ${CMAKE_CURRENT_LIST_FILE} PATH)

# Enable tools individually
option(TOOL_INTERPRETER "Build tests" OFF)

if(TOOL_INTERPRETER)
    include(${ToolsDir}/Interpreter/Interpreter.cmake)
endif()
