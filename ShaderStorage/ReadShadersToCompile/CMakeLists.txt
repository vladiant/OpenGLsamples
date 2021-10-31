cmake_minimum_required(VERSION 3.10)

project(ReadShadersToCompile LANGUAGES C CXX)

find_package(GLUT REQUIRED)
find_package(GLEW REQUIRED)
find_package(OpenGL REQUIRED COMPONENTS OpenGL)
find_package(PkgConfig)

pkg_check_modules(GLSLANG REQUIRED glslang)
pkg_check_modules(SPIRV_TOOLS REQUIRED SPIRV-Tools)

find_program(GLSLANG_VALIDATOR NAMES glslangValidator)

file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/shaders)
configure_file(shaders/simple_shader.frag ./shaders/simple_shader.frag COPYONLY)
configure_file(shaders/simple_shader.vert ./shaders/simple_shader.vert COPYONLY)

add_executable(
    ${PROJECT_NAME}
    util.hpp
    util.cpp
    main.cpp
)

set_target_properties(
    ${PROJECT_NAME}
    PROPERTIES
        CXX_STANDARD 17
        CXX_STANDARD_REQUIRED YES
        CXX_EXTENSIONS NO
)

target_include_directories(
    ${PROJECT_NAME}
    PRIVATE
        ${GLSLANG_INCLUDE_DIRS}
        ${SPIRV_TOOLS_INCLUDE_DIRS}
)

target_link_libraries(
    ${PROJECT_NAME}
    PRIVATE
        ${GLSLANG_LIBRARIES}
        SPIRV
        ${SPIRV_TOOLS_LIBRARIES}
        pthread
        GLUT::GLUT
        GLEW::GLEW
        OpenGL::GL
)

