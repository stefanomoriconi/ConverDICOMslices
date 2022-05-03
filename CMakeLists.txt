cmake_minimum_required(VERSION 3.3)
project(ConvertDICOMslices)

find_package(ITK REQUIRED)
include(${ITK_USE_FILE})

add_executable(ConvertDICOMslices ConvertDICOMslices.cpp ConvertDICOMslices.h)
target_link_libraries(ConvertDICOMslices ${ITK_LIBRARIES})
