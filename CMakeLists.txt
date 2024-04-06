cmake_minimum_required(VERSION 3.13)

if(POLICY CMP0077)
    cmake_policy(SET CMP0077 NEW)
endif()

project(Sleipnir C ASM)

set(CMAKE_C_STANDARD 11)

include(CheckIncludeFile)
include(CheckSymbolExists)
include(GNUInstallDirs)

set(SLEIPNIR_HDRS)
set(SLEIPNIR_SRCS)


file(GLOB HDRS_SOURCES CONFIGURE_DEPENDS "${PROJECT_SOURCE_DIR}/include/*.h")
list(APPEND SLEIPNIR_HDRS ${HDRS_SOURCES})

file(GLOB IMPL_SOURCES CONFIGURE_DEPENDS "${PROJECT_SOURCE_DIR}/src/*.c")
list(APPEND SLEIPNIR_SRCS ${IMPL_SOURCES})

#
# library : sleipnir
#

add_library(sleipnir ${SLEIPNIR_HDRS} ${SLEIPNIR_SRCS})
set_target_properties(sleipnir PROPERTIES PUBLIC_HEADER "${SLEIPNIR_HDRS}")
target_include_directories(sleipnir
        PUBLIC  $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
        PRIVATE $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include/internal>
)
target_link_libraries(sleipnir PUBLIC ${CMAKE_DL_LIBS} sleipnir_lib)
add_library(Sleipnir::sleipnir ALIAS sleipnir)

#
# program : sleipnir_demo
#

add_executable(sleipnir_demo ${PROJECT_SOURCE_DIR}/src/utils/sleipnir_demo.c)
target_link_libraries(sleipnir_demo PRIVATE sleipnir sleipnir_lib)
add_executable(Sleipnir::sleipnir_demo ALIAS sleipnir_demo)

include(CTest)
if(BUILD_TESTING)
    enable_language(CXX)

    set(CMAKE_CXX_STANDARD 14)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
    set(CMAKE_CXX_EXTENSIONS OFF)

    if(NOT TARGET gtest OR NOT TARGET gmock_main)
        # Download googletest.
        configure_file(
                cmake/googletest.CMakeLists.txt.in
                googletest-download/CMakeLists.txt
        )

        execute_process(
                COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
                RESULT_VARIABLE result
                WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/googletest-download)

        if(result)
            message(FATAL_ERROR "CMake step for googletest failed: ${result}")
        endif()

        execute_process(
                COMMAND ${CMAKE_COMMAND} --build .
                RESULT_VARIABLE result
                WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/googletest-download)

        if(result)
            message(FATAL_ERROR "Build step for googletest failed: ${result}")
        endif()

        # Add googletest directly to build.
        add_subdirectory(${CMAKE_BINARY_DIR}/googletest-src
                ${CMAKE_BINARY_DIR}/googletest-build
                EXCLUDE_FROM_ALL)
    endif()

    add_subdirectory(test)
endif()

if(NOT TARGET sleipnir_lib)
    configure_file(
            cmake/sleipnir-lib.CMakeLists.txt.in
            libsleipnir-download/CMakeLists.txt
    )

    execute_process(
            COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
            RESULT_VARIABLE result
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/libsleipnir-download)

    if(result)
        message(FATAL_ERROR "CMake step for libsleipnir failed: ${result}")
    endif()

    execute_process(
            COMMAND ${CMAKE_COMMAND} --build .
            RESULT_VARIABLE result
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/libsleipnir-download)

    if(result)
        message(FATAL_ERROR "Build step for libsleipnir failed: ${result}")
    endif()

    # Add libsleipnir directly to build.
    add_subdirectory(${CMAKE_BINARY_DIR}/libsleipnir-src
            ${CMAKE_BINARY_DIR}/libsleipnir-build
            EXCLUDE_FROM_ALL)
endif()

add_subdirectory(benchmark)