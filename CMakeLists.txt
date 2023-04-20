cmake_minimum_required(VERSION 3.12)

set(CMAKE_GENERATOR "Visual Studio 17 2022")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

project(Tutorial_Adventure)

# if another Vulkan installation wants to be used specify VULKAN_SDK_PATH
if(DEFINED VULKAN_SDK_PATH)
    set(Vulkan_INCLUDE_DIRS "${Vulkan_SDK_PATH}/Include")
    set(Vulkan_LIBRARIES "${Vulkan_SDK_PATH}/Lib")
    set(Vulkan_FOUND "True")
else()
    find_package(Vulkan REQUIRED)
endif()
if(NOT Vulkan_FOUND)
    message(FATAL_ERROR "Can't find the Vulkan library!")
else()
    message(STATUS "Found Vulkan library: ${Vulkan_LIBRARIES}")
endif()

add_subdirectory(src/Tutorial_Adventure)

set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT Tutorial_Adventure)