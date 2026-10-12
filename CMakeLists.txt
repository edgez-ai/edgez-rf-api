cmake_minimum_required(VERSION 3.20)

project(edgez_rf_api LANGUAGES C)

add_library(edgez_rf_api INTERFACE)
target_include_directories(edgez_rf_api INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_compile_features(edgez_rf_api INTERFACE c_std_11)
