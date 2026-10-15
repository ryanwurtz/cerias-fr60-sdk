cmake_minimum_required(VERSION 3.20)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
project(fr60_sdk LANGUAGES C)

add_library(fr60_sdk INTERFACE)
target_include_directories(fr60_sdk INTERFACE
    "${CMAKE_CURRENT_SOURCE_DIR}/sdk/include"
)

set(FR60_TEST_PROGRAM "multest" CACHE STRING "Test source name in tests/src")
if(NOT FR60_TEST_PROGRAM MATCHES "^[A-Za-z0-9_]+$")
    message(FATAL_ERROR "Invalid test program name: ${FR60_TEST_PROGRAM}")
endif()

add_executable(${FR60_TEST_PROGRAM} tests/src/${FR60_TEST_PROGRAM}.c)
target_link_libraries(${FR60_TEST_PROGRAM} PRIVATE fr60_sdk gcc)
target_compile_options(${FR60_TEST_PROGRAM} PRIVATE -O0 -g -ffreestanding)
target_link_options(${FR60_TEST_PROGRAM} PRIVATE
    -nostdlib -Wl,-e,main -Wl,-Ttext=0x10000
)
set_target_properties(${FR60_TEST_PROGRAM} PROPERTIES
    SUFFIX ".elf"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/tests/output/${FR60_TEST_PROGRAM}"
)
