cmake_minimum_required(VERSION 3.5)
set(COMPONENTS esptool_py esp32 main esp32_hap)
include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(mosquito)