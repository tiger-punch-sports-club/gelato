cmake_minimum_required(VERSION 3.0)
project(gelato89)

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

# gelato89 renderer lib
add_library(
    gelato89
    ${PROJECT_SOURCE_DIR}/texture.c
    ${PROJECT_SOURCE_DIR}/platform.c
    ${PROJECT_SOURCE_DIR}/renderer.c
    ${PROJECT_SOURCE_DIR}/sprite.c
    ${PROJECT_SOURCE_DIR}/utils.c
)
target_link_libraries(gelato89 PUBLIC ${OPENGL_gl_LIBRARY} PRIVATE glew)

# project sandbox
project(gelato89-sandbox CXX)
SET (PROJECT_SOURCE_DIR "${PROJECT_SOURCE_DIR}/sandbox")
SET (GLFW_SOURCE_DIR "vendor/glfw-3.3")

# glfw 3.3
add_subdirectory(${GLFW_SOURCE_DIR})

# gelato89-sandbox cpp application
add_executable(
                gelato89-sandbox 
                ${PROJECT_SOURCE_DIR}/sandbox.cpp
                ${PROJECT_SOURCE_DIR}/stb_image.c
)
target_include_directories(gelato89-sandbox PRIVATE ${GLFW_SOURCE_DIR}/include)
target_link_libraries(gelato89-sandbox PUBLIC glfw ${GLFW_LIBRARIES} PRIVATE gelato89)