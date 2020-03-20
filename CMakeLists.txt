cmake_minimum_required(VERSION 3.0)
project(gelato)

SET (PROJECT_SOURCE_DIR "${PROJECT_SOURCE_DIR}/source")
SET (EXTERNALS_INCLUDE_DIR "${PROJECT_SOURCE_DIR}/external")
SET (GLEW_DIR "${PROJECT_SOURCE_DIR}/external/glew-2.1.0")

include_directories(${PROJECT_SOURCE_DIR} ${EXTERNALS_INCLUDE_DIR} ${GLEW_DIR}/include)
MESSAGE ("Source path: ${PROJECT_SOURCE_DIR}")

find_package(OpenGL REQUIRED)

add_library(
    glew
    ${GLEW_DIR}/src/glew.c
)
target_compile_definitions(glew PUBLIC GLEW_STATIC)

# gelato renderer lib
add_library(
    gelato
    ${PROJECT_SOURCE_DIR}/texture.c
    ${PROJECT_SOURCE_DIR}/platform.c
    ${PROJECT_SOURCE_DIR}/renderer.c
    ${PROJECT_SOURCE_DIR}/sprite.c
    ${PROJECT_SOURCE_DIR}/utils.c
)
target_link_libraries(gelato PUBLIC ${OPENGL_gl_LIBRARY} PRIVATE glew)

# project sandbox
project(gelato-sandbox CXX)
SET (PROJECT_SOURCE_DIR "${PROJECT_SOURCE_DIR}/sandbox")
SET (GLFW_SOURCE_DIR "vendor/glfw-3.3")

# glfw 3.3
add_subdirectory(${GLFW_SOURCE_DIR})

# gelato-sandbox cpp application
add_executable(
                gelato-sandbox 
                ${PROJECT_SOURCE_DIR}/sandbox.cpp
                ${PROJECT_SOURCE_DIR}/stb_image.c
)
target_include_directories(gelato-sandbox PRIVATE ${GLFW_SOURCE_DIR}/include)
target_link_libraries(gelato-sandbox PUBLIC glfw ${GLFW_LIBRARIES} PRIVATE gelato)