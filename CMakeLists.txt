cmake_minimum_required(VERSION 3.10)

add_library(project_settings INTERFACE)
target_compile_features(project_settings INTERFACE cxx_std_20)

project(playground)

add_subdirectory(src)
