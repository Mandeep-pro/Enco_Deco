
cmake_minimum_required(VERSION 3.16)

project(Enco_Deco LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(PkgConfig REQUIRED)
pkg_check_modules(RAYLIB REQUIRED IMPORTED_TARGET raylib)

add_executable(enco_deco
    src/main.cpp
    src/ui_helpers.cpp
    src/input_panel.cpp
    src/waveform_renderer.cpp
    src/line_encoder.cpp
    src/sequence_generator.cpp
    src/palindrome.cpp
    src/ami_scrambler.cpp
    src/delta_modulation.cpp
src/analog_signal_generator.cpp
    src/delta_modulation_panel.cpp
)

target_include_directories(enco_deco PRIVATE include)
target_link_libraries(enco_deco PRIVATE PkgConfig::RAYLIB)

target_compile_options(enco_deco PRIVATE
    -Wall
    -Wextra
    -Wpedantic
)

set_target_properties(enco_deco PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

include(CTest)

if(BUILD_TESTING)
    function(add_project_test test_name test_source)
        add_executable(${test_name}
            ${test_source}
            ${ARGN}
        )

        target_include_directories(${test_name} PRIVATE include)

        target_compile_options(${test_name} PRIVATE
            -Wall -Wextra -Wpedantic
        )

        add_test(NAME ${test_name} COMMAND ${test_name})
    endfunction()

    add_project_test(
        line_encoder_test
        tests/line_encoder_test.cpp
        src/line_encoder.cpp
    )

    add_project_test(
        sequence_generator_test
        tests/sequence_generator_test.cpp
        src/sequence_generator.cpp
    )

    add_project_test(
        palindrome_test
        tests/palindrome_test.cpp
        src/palindrome.cpp
    )

    add_project_test(
        ami_scrambler_test
        tests/ami_scrambler_test.cpp
        src/ami_scrambler.cpp
    )

    add_project_test(
        analog_signal_generator_test
        tests/analog_signal_generator_test.cpp
        src/analog_signal_generator.cpp
    )

    add_project_test(
        delta_modulation_test
        tests/delta_modulation_test.cpp
        src/delta_modulation.cpp
    )

    add_project_test(
        delta_modulation_integration_test
        tests/delta_modulation_integration_test.cpp
        src/delta_modulation.cpp
        src/analog_signal_generator.cpp
    )
endif()
