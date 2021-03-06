project(Interpreter)

get_filename_component(InterpreterDir ${CMAKE_CURRENT_LIST_FILE} PATH)

find_package(Threads REQUIRED)

set(InterpreterSources
    ${InterpreterDir}/Interpreter.hpp
    ${InterpreterDir}/Interpreter.ipp
    ${InterpreterDir}/Interpreter.cpp
    ${InterpreterDir}/Scheduler.hpp
    ${InterpreterDir}/Main.cpp
)

add_executable(${PROJECT_NAME} ${InterpreterSources})

target_link_libraries(${PROJECT_NAME}
PUBLIC
    Audio
    Threads::Threads
    PartitionDecoder
)