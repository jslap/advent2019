cmake_minimum_required(VERSION 3.10)
project(my_project)

set(CMAKE_CXX_STANDARD 17)

add_subdirectory(abseil-cpp)
add_subdirectory(fmt)
add_subdirectory(range-v3)


set(CMAKE_CXX_FLAGS "-O0 -g ${CMAKE_CXX_FLAGS} -Werror -Wall -Wpedantic -Wno-unused-variable -Wno-unused-but-set-variable")

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    add_compile_options (-fdiagnostics-color=always)
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    add_compile_options (-fcolor-diagnostics)
endif ()

add_executable(my_exe advent14.cpp intMachine.cpp)

set_property(TARGET my_exe PROPERTY CXX_STANDARD 17)

target_include_directories(my_exe SYSTEM PRIVATE "range-v3/include")
target_include_directories(my_exe SYSTEM PRIVATE "fmt/include")
target_include_directories(my_exe SYSTEM PRIVATE "abseil-cpp")

target_link_libraries(my_exe absl::base absl::strings absl::numeric fmt)
