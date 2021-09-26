cmake_minimum_required(VERSION 3.10)

project(Windowless LANGUAGES C)

find_package(PkgConfig)
find_package(OpenGL REQUIRED COMPONENTS OpenGL EGL)

pkg_check_modules(GBM REQUIRED gbm)

add_executable(
    ${PROJECT_NAME}
    main.c
)

set_target_properties(
    ${PROJECT_NAME}
    PROPERTIES
        C_STANDARD 11
        C_STANDARD_REQUIRED YES
        C_EXTENSIONS NO
)

target_include_directories(
    ${PROJECT_NAME}
    PRIVATE
        ${GBM_INCLUDE_DIRS}
)

target_link_libraries(
    ${PROJECT_NAME}
    PRIVATE
        OpenGL::GL
        OpenGL::EGL
        ${GBM_LIBRARIES}
)
