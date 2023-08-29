cmake_minimum_required(VERSION 3.22)
project(OpenMP-Quantum)

# Clang must be built with OpenMP runtime
set(CMAKE_C_COMPILER clang)
set(CMAKE_CXX_COMPILER clang++)

set(PYTHON_DIR "" CACHE PATH "Path to Python root directory")
set(NLOPT_DIR "" CACHE PATH "Path to nlopt root directory")
set(RABBITMQ_DIR "" CACHE PATH "Path to rabbitmq-c root directory")
set(JANSSON_DIR "" CACHE PATH "Path to jansson root directory")

list(APPEND CMAKE_PREFIX_PATH
    ${PYTHON_DIR}
    ${NLOPT_DIR}
    ${RABBITMQ_DIR}
    ${JANSSON_DIR}
)

find_package(rabbitmq-c REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_check_modules(NLOPT REQUIRED nlopt)
pkg_check_modules(JANSSON REQUIRED jansson)
find_package(Python3 3.10 COMPONENTS Interpreter Development REQUIRED)

add_subdirectory(src)
add_subdirectory(examples)

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/include DESTINATION ${CMAKE_BINARY_DIR})