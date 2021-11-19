cmake_minimum_required(VERSION 3.10)

project(WriteBufferFromStream LANGUAGES C CXX)

find_package(PNG REQUIRED)
find_package(GLUT REQUIRED)
find_package(GLEW REQUIRED)
find_package(OpenGL REQUIRED COMPONENTS OpenGL)

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
        GLUT::GLUT
        GLEW::GLEW
        OpenGL::GL
        PNG::PNG
)
