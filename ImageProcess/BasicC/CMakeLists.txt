cmake_minimum_required(VERSION 3.10)

project(BasicC LANGUAGES C)

find_package(PNG REQUIRED)
find_package(glfw3 3.3 REQUIRED)
find_package(OpenGL REQUIRED COMPONENTS OpenGL)

add_executable(
    ${PROJECT_NAME}
    png.h
    png.c
    image.h
    image.c
    main.c
)

set_target_properties(
    ${PROJECT_NAME}
    PROPERTIES
        C_STANDARD 11
        C_STANDARD_REQUIRED YES
        C_EXTENSIONS NO
)

target_link_libraries(
    ${PROJECT_NAME}
    PRIVATE
        glfw
        OpenGL::GL
        PNG::PNG
)
