cmake_minimum_required(VERSION 3.20)
project(fr60_sdk LANGUAGES C ASM)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON) # clangd intellisense support

set(FR60_OPT_LEVEL "0" CACHE STRING "GCC optimization level for FR60 C sources")
if(NOT FR60_OPT_LEVEL MATCHES "^([0123gsz]|fast)$")
    message(FATAL_ERROR "FR60_OPT_LEVEL must be 0, 1, 2, 3, g, s, z, or fast")
endif()

# build sdk library
add_library(fr60_sdk STATIC 
    src/syscalls.c
    src/uart.c)
target_include_directories(fr60_sdk PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
)
target_compile_options(fr60_sdk PRIVATE "-O${FR60_OPT_LEVEL}" -g -ffreestanding -Wall -Wextra
    -ffunction-sections -fdata-sections)

# add program target
option(FR60_INCLUDE_STARTUP "Include crt0.S runtime initialization" OFF)
if(NOT DEFINED FR60_TEST_PROGRAM)
    set(FR60_TEST_PROGRAM "multest")
endif()
add_executable(${FR60_TEST_PROGRAM}
    "${CMAKE_CURRENT_SOURCE_DIR}/../tests/src/${FR60_TEST_PROGRAM}.c"
)
if(FR60_INCLUDE_STARTUP)
    target_sources(${FR60_TEST_PROGRAM} PRIVATE src/crt0.S)
else()
    # Override ENTRY(_start) and retain main when linking with --gc-sections.
    target_link_options(${FR60_TEST_PROGRAM} PRIVATE -Wl,-e,main)
endif()

# compiler/linker options
target_link_libraries(${FR60_TEST_PROGRAM} PRIVATE
    -Wl,--start-group fr60_sdk m c gcc -Wl,--end-group
)
target_compile_options(${FR60_TEST_PROGRAM} PRIVATE "$<$<COMPILE_LANGUAGE:C>:-O${FR60_OPT_LEVEL}>" -g -ffreestanding
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
# Libraries passed by name in the link group need explicit file dependencies.
foreach(archive IN ITEMS libc.a libm.a)
    execute_process(
        COMMAND "${CMAKE_C_COMPILER}" "-print-file-name=${archive}"
        OUTPUT_VARIABLE archive_path
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    if(IS_ABSOLUTE "${archive_path}")
        set_property(TARGET ${FR60_TEST_PROGRAM} APPEND PROPERTY
            LINK_DEPENDS "${archive_path}"
        )
    endif()
endforeach()
set_target_properties(${FR60_TEST_PROGRAM} PROPERTIES
    SUFFIX ".elf"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/../tests/output/${FR60_TEST_PROGRAM}"
)
