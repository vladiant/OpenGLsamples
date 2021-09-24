cmake_minimum_required(VERSION 3.10)

project(Windowless LANGUAGES C)

find_package(OpenGL REQUIRED COMPONENTS OpenGL EGL)

add_executable(
    ${PROJECT_NAME}
    main.c
)

set_target_properties(
    ${PROJECT_NAME}
    PROPERTIES
        C_STANDARD 17
        C_STANDARD_REQUIRED YES
        C_EXTENSIONS NO
)

target_link_libraries(
    ${PROJECT_NAME}
    PRIVATE
        OpenGL::GL
        OpenGL::EGL
        gbm
)
