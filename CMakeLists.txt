cmake_minimum_required(VERSION 3.16)

add_compile_options(-std=c++17)

project(chxser VERSION 0.0.0)

find_package(chxstl REQUIRED)

add_executable(chxser "src/main.cpp")

target_link_libraries(chxser PUBLIC chxstl)
target_compile_features(chxser PRIVATE cxx_std_17)
