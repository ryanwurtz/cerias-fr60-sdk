cmake_minimum_required(VERSION 3.20)
project(fr60_sdk LANGUAGES C ASM)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON) # clangd intellisense support

# build sdk library
add_library(fr60_sdk STATIC 
    src/syscalls.c
    src/uart.c)
target_include_directories(fr60_sdk PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
)
target_compile_options(fr60_sdk PRIVATE -g -ffreestanding -Wall -Wextra
    -ffunction-sections -fdata-sections)

# add program target
if(NOT DEFINED FR60_TEST_PROGRAM)
    set(FR60_TEST_PROGRAM "multest")
endif()
add_executable(${FR60_TEST_PROGRAM}
    src/crt0.S
    "${CMAKE_CURRENT_SOURCE_DIR}/../tests/src/${FR60_TEST_PROGRAM}.c"
)

# compiler/linker options
target_link_libraries(${FR60_TEST_PROGRAM} PRIVATE
    -Wl,--start-group fr60_sdk m c gcc -Wl,--end-group
)
target_compile_options(${FR60_TEST_PROGRAM} PRIVATE -O0 -g -ffreestanding
    -ffunction-sections -fdata-sections)
target_link_options(${FR60_TEST_PROGRAM} PRIVATE
    -nostdlib
    -Wl,--gc-sections
    "-Wl,-T,${CMAKE_CURRENT_SOURCE_DIR}/linker.ld"
    "-Wl,-Map,${CMAKE_CURRENT_BINARY_DIR}/${FR60_TEST_PROGRAM}.map"
)
set_property(TARGET ${FR60_TEST_PROGRAM} APPEND PROPERTY
    LINK_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/linker.ld"
)
set_target_properties(${FR60_TEST_PROGRAM} PROPERTIES
    SUFFIX ".elf"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/../tests/output/${FR60_TEST_PROGRAM}"
)
