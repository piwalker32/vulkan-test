cmake_minimum_required(VERSION 3.10)
project(vulkantest VERSION 0.1)
FILE(GLOB Sources src/*.cpp)
FILE(GLOB Resources res/*)
add_executable(vulkantest ${Sources})
target_compile_definitions(vulkantest PRIVATE
    $<$<CONFIG:Release>:
        NDEBUG=1
    >
)
add_custom_command(TARGET vulkantest POST_BUILD COMMAND ${CMAKE_COMMAND} ARGS -E rm -rf ${CMAKE_BINARY_DIR}/res)
add_custom_command(TARGET vulkantest POST_BUILD COMMAND ${CMAKE_COMMAND} ARGS -E copy_directory ${CMAKE_SOURCE_DIR}/res/ ${CMAKE_BINARY_DIR}/res)
#FILE(COPY ${Resources} DESTINATION ${CMAKE_BINARY_DIR}/res)
target_link_libraries(vulkantest glfw glm vulkan dl pthread X11 Xxf86vm Xrandr Xi)