cmake_minimum_required(VERSION 3.10)

project(HelloEGL LANGUAGES CXX)

find_package(X11 REQUIRED)
find_package(OpenGL REQUIRED COMPONENTS OpenGL EGL)

add_executable(
    ${PROJECT_NAME}
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
        OpenGL::EGL
)

