project("Tutorial_Adventure")

include_directories(Input)
include_directories(${Vulkan_INCLUDE_DIRS})
include_directories(${CMAKE_SOURCE_DIR}/vendor/glfw-3.3.8/include)
include_directories(${CMAKE_SOURCE_DIR}/vendor/glm-0.9.9.8/glm)
include_directories(${CMAKE_SOURCE_DIR}/vendor/STB/include)

link_directories(${CMAKE_SOURCE_DIR}/vendor/glfw-3.3.8/lib-vc2022)

file(GLOB TUTORIAL_ADVENTURE_SRC CONFIGURE_DEPENDS "*.h" "*.cpp" "Input/*.h" "Input/*.cpp")

add_executable(Tutorial_Adventure ${TUTORIAL_ADVENTURE_SRC})
target_link_libraries(Tutorial_Adventure ${Vulkan_LIBRARIES})
target_link_libraries(Tutorial_Adventure glfw3.lib)