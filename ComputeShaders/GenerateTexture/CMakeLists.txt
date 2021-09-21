cmake_minimum_required(VERSION 3.10)

project(GenerateTexture LANGUAGES CXX)

find_package(OpenGL REQUIRED COMPONENTS OpenGL GLX)
find_package(X11 REQUIRED)

add_executable(
    ${PROJECT_NAME}
    opengl.hpp
    opengl_cs.cpp
    opengl_util.cpp
    main.cpp
)

set_target_properties(
    ${PROJECT_NAME}
    PROPERTIES
        CXX_STANDARD 17
        CXX_STANDARD_REQUIRED YES
        CXX_EXTENSIONS NO
)

target_link_libraries(
    ${PROJECT_NAME}
    PRIVATE
        ${X11_LIBRARIES}
        OpenGL::GL
        OpenGL::GLU
        OpenGL::GLX
)
