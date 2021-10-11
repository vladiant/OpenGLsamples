cmake_minimum_required(VERSION 3.10)

project(ReadShaders LANGUAGES C CXX)

find_package(GLUT REQUIRED)
find_package(GLEW REQUIRED)
find_package(OpenGL REQUIRED COMPONENTS OpenGL)

file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/shaders)
configure_file(shaders/simple_shader.frag ./shaders/simple_shader.frag COPYONLY)
configure_file(shaders/simple_shader.vert ./shaders/simple_shader.vert COPYONLY)

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
)
