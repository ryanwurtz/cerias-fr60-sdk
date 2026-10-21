cmake_minimum_required(VERSION 3.20)
project(fr60_sdk LANGUAGES C)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON) # clangd intellisense support

# build sdk library
add_library(fr60_sdk STATIC 
    sdk/src/syscalls.c
    sdk/src/uart.c)
target_include_directories(fr60_sdk PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/sdk/include"
)
target_compile_options(fr60_sdk PRIVATE -g -ffreestanding -Wall -Wextra)

# add program target
if(NOT DEFINED FR60_TEST_PROGRAM)
    set(FR60_TEST_PROGRAM "multest")
endif()
add_executable(${FR60_TEST_PROGRAM} tests/src/${FR60_TEST_PROGRAM}.c)

# compiler/linker options
target_link_libraries(${FR60_TEST_PROGRAM} PRIVATE
    -Wl,--start-group fr60_sdk m c gcc -Wl,--end-group
)
target_compile_options(${FR60_TEST_PROGRAM} PRIVATE -O0 -g -ffreestanding)
target_link_options(${FR60_TEST_PROGRAM} PRIVATE
    -nostdlib -Wl,-e,main -Wl,-Ttext=0x10000
)
set_target_properties(${FR60_TEST_PROGRAM} PROPERTIES
    SUFFIX ".elf"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/tests/output/${FR60_TEST_PROGRAM}"
)