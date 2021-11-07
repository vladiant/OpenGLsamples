// https://github.com/elima/gpu-playground/tree/master/gl-image-loader

#include <GLES2/gl2.h>
#include <GLFW/glfw3.h>

#include <cassert>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <png++/png.hpp>
#include <string>
#include <vector>

bool gl_utils_print_shader_log(GLuint shader) {
  GLint length;
  char buffer[4096] = {0};
  GLint success;

  glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
  if (length == 0) return true;

  glGetShaderInfoLog(shader, sizeof(buffer), NULL, buffer);
  if (strlen(buffer) > 0) printf("Shader compilation log: %s\n", buffer);

  glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

  return success == GL_TRUE;
}

GLuint gl_utils_load_shader(const char *shader_source, GLenum type) {
  GLuint shader = glCreateShader(type);

  glShaderSource(shader, 1, &shader_source, NULL);
  assert(glGetError() == GL_NO_ERROR);
  glCompileShader(shader);
  assert(glGetError() == GL_NO_ERROR);

  gl_utils_print_shader_log(shader);

  return shader;
}

GLuint create_shader_program() {
  constexpr char VERTEX_SOURCE[] =
      "attribute vec2 pos;\n"
      "attribute vec2 texture;\n"
      "varying vec2 v_texture;\n"
      "void main() {\n"
      "  v_texture = texture;\n"
      "  gl_Position = vec4(pos, 0, 1);\n"
      "}\n";

  constexpr char FRAGMENT_SOURCE[] =
      "precision mediump float;\n"
      "uniform sampler2D u_tex;\n"
      "varying vec2 v_texture;\n"
      "void main() {\n"
      "  gl_FragColor = texture2D(u_tex, v_texture);\n"
      "}\n";

  GLuint vertex_shader = gl_utils_load_shader(VERTEX_SOURCE, GL_VERTEX_SHADER);
  assert(vertex_shader >= 0);
  assert(glGetError() == GL_NO_ERROR);

  GLuint fragment_shader =
      gl_utils_load_shader(FRAGMENT_SOURCE, GL_FRAGMENT_SHADER);
  assert(fragment_shader >= 0);
  assert(glGetError() == GL_NO_ERROR);

  GLuint program = glCreateProgram();
  assert(glGetError() == GL_NO_ERROR);
  glAttachShader(program, vertex_shader);
  assert(glGetError() == GL_NO_ERROR);
  glAttachShader(program, fragment_shader);
  assert(glGetError() == GL_NO_ERROR);

  glBindAttribLocation(program, 0, "pos");
  glBindAttribLocation(program, 1, "texture");

  glLinkProgram(program);
  assert(glGetError() == GL_NO_ERROR);

  glDeleteShader(vertex_shader);
  glDeleteShader(fragment_shader);

  return program;
}

int32_t main(int32_t argc, char *argv[]) {
  printf("Usage: %s <path-to-PNG-image>\n", argv[0]);

  if (argc <= 1) return EXIT_FAILURE;

  // Load an decode an image.
  png::image<png::rgb_pixel> image(argv[1]);

  GLFWwindow *window;

  // Initialize GLFW.
  if (!glfwInit()) return EXIT_FAILURE;

  // Select an OpenGL-ES 2.0 profile.
  glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

  // Create a windowed mode window and its OpenGL context
  window = glfwCreateWindow(image.get_width(), image.get_height(),
                            "GL Image Loader", NULL, NULL);
  if (window == NULL) {
    glfwTerminate();
    return EXIT_FAILURE;
  }

  // Make the window's context current
  glfwMakeContextCurrent(window);

  // Dump some GL capabilities.
  const GLubyte *gles_version = glGetString(GL_VERSION);
  printf("%s\n", (char *)gles_version);

  // Create a texture for the image.
  GLuint tex;
  glGenTextures(1, &tex);
  assert(glGetError() == GL_NO_ERROR);
  assert(tex > 0);
  glBindTexture(GL_TEXTURE_2D, tex);
  assert(glGetError() == GL_NO_ERROR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  // Load the image into the texture
  std::vector<uint8_t> buf(image.get_width() * image.get_height() * 3);
  GLuint format = GL_RGB;  // png::rgb_pixel

  ssize_t size_read = 0;
  for (size_t y = 0; y < image.get_height(); ++y) {
    for (size_t x = 0; x < image.get_width(); ++x) {
      buf[size_read++] = image[y][x].red;
      buf[size_read++] = image[y][x].green;
      buf[size_read++] = image[y][x].blue;
    }
  }

  // Allocate the texture size.
  glTexImage2D(GL_TEXTURE_2D, 0, format, image.get_width(), image.get_height(),
               0, format, GL_UNSIGNED_BYTE, buf.data());
  assert(glGetError() == GL_NO_ERROR);

  glBindTexture(GL_TEXTURE_2D, 0);

  // Create shader program to sample the texture.
  GLuint program = create_shader_program();
  glUseProgram(program);
  assert(glGetError() == GL_NO_ERROR);

  // Loop until the user closes the window
  while (!glfwWindowShouldClose(window)) {
    // Render here
    glClearColor(0.25, 0.25, 0.25, 0.5);
    glClear(GL_COLOR_BUFFER_BIT);

    // Bind the texture.
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tex);
    assert(glGetError() == GL_NO_ERROR);

    // Enable blending for transparent PNGs.
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Draw a quad.
    constexpr GLfloat s_vertices[4][2] = {
        {-1.0, 1.0},
        {1.0, 1.0},
        {-1.0, -1.0},
        {1.0, -1.0},
    };

    constexpr GLfloat s_texturePos[4][2] = {
        {0, 0},
        {1, 0},
        {0, 1},
        {1, 1},
    };

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, s_vertices);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, s_texturePos);

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);

    // Swap front and back buffers
    glfwSwapBuffers(window);

    // Poll and process events
    glfwPollEvents();
  }

  glfwTerminate();

  return EXIT_SUCCESS;
}
