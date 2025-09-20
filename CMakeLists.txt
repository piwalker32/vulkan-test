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
target_include_directories(vulkantest PRIVATE include/)
add_custom_command(TARGET vulkantest POST_BUILD COMMAND ${CMAKE_COMMAND} ARGS -E rm -rf ${CMAKE_BINARY_DIR}/res)
add_custom_command(TARGET vulkantest POST_BUILD COMMAND ${CMAKE_COMMAND} ARGS -E copy_directory ${CMAKE_SOURCE_DIR}/res/ ${CMAKE_BINARY_DIR}/res)
#FILE(COPY ${Resources} DESTINATION ${CMAKE_BINARY_DIR}/res)
target_link_libraries(vulkantest glfw glm vulkan dl pthread X11 Xxf86vm Xrandr Xi)
set( ${CMAKE_CURRENT_SOURCE_DIR}/shaders)

set(SHADER_DIR ${CMAKE_CURRENT_SOURCE_DIR}/shaders)
file(GLOB SHADERS ${SHADER_DIR}/*.vert ${SHADER_DIR}/*.frag ${SHADER_DIR}/*.comp ${SHADER_DIR}/*.geom ${SHADER_DIR}/*.tesc ${SHADER_DIR}/*.tese ${SHADER_DIR}/*.mesh ${SHADER_DIR}/*.task ${SHADER_DIR}/*.rgen ${SHADER_DIR}/*.rchit ${SHADER_DIR}/*.rmiss)

find_package(Vulkan)

foreach(SHADER IN LISTS SHADERS)
    get_filename_component(FILENAME ${SHADER} NAME)
    file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/shaders)
    add_custom_command(OUTPUT ${SHADER_DIR}/shaders/${FILENAME}.spv
        COMMAND ${Vulkan_GLSLC_EXECUTABLE} ${SHADER} -o ${CMAKE_BINARY_DIR}/shaders/${FILENAME}.spv
        DEPENDS ${SHADER}
        COMMENT "Compiling ${FILENAME}")
list(APPEND SPV_SHADERS ${SHADER_DIR}/shaders/${FILENAME}.spv)
endForeach()

add_custom_target(shaders ALL DEPENDS ${SPV_SHADERS})

add_dependencies(vulkantest shaders)