
// https://www.geeksforgeeks.org/rendering-triangle-using-openglusing-shaders/
// https://github.com/hikiko/gl4

#include <GL/glew.h>
#include <GL/glut.h>
#include <GL/glx.h>

#include <cmath>
#include <fstream>
#include <iostream>
#include <vector>

std::vector<char> readFile(const std::string& filepath) {
  std::ifstream file{filepath, std::ios::binary};

  if (!file) {
    throw std::runtime_error("failed to open file: " + filepath);
  }

  std::vector<char> result{std::istreambuf_iterator<char>(file),
                           std::istreambuf_iterator<char>()};

  return result;
}

void display() {
  // clean the back buffer and assign new color to it
  glClear(GL_COLOR_BUFFER_BIT);

  // Draw the triangle using the GL_TRIANGLES primitive
  glDrawArrays(GL_TRIANGLES, 0, 3);

  // Swap the buffers and hence show the buffers
  // content to the screen
  glutSwapBuffers();
};

static PFNGLSPECIALIZESHADERPROC gl_specialize_shader;

int main(int argc, char** argv) {
  gl_specialize_shader = (PFNGLSPECIALIZESHADERPROC)glXGetProcAddress(
      (unsigned char*)"glSpecializeShaderARB");
  if (!gl_specialize_shader) {
    fprintf(stderr, "failed to load glSpecializeShaderARB entry point\n");
    return EXIT_FAILURE;
  }

  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
  glutInitWindowSize(500, 500);
  glutInitWindowPosition(100, 50);
  glutCreateWindow("Triangle");

  glewInit();

  // Vertices coordinates
  GLfloat vertices[] = {0.0f,  0.5f,  0.0f,   // top
                        -0.5f, -0.5f, 0.0f,   // left
                        0.5f,  -0.5f, 0.0f};  // right

  // Create reference container for the Vertex Buffer Object
  GLuint VBO;
  // Generate the VBO with only one object
  glGenBuffers(1, &VBO);
  // Bind the VBO specifying it's a GL_ARRAY_BUFFER
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  // Introduce the vertices into the VBO
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
  // Bind the VBO to 0 so that accidental modification is excluded
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  // Read Vertex Shader source code
  const auto vertexShaderData = readFile("shaders/simple_shader.vert.spv");
  const auto vertexShaderSource = vertexShaderData.data();
  // Create Vertex Shader Object and get reference
  GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
  // Attach the compiled binary shader
  glShaderBinary(1, &vertexShader, GL_SHADER_BINARY_FORMAT_SPIR_V_ARB,
                 vertexShaderSource, vertexShaderData.size());
  gl_specialize_shader(vertexShader, "main", 0, 0, 0);

  // Read Vertex Shader binary code
  const auto fragmentShaderData = readFile("shaders/simple_shader.frag.spv");
  const auto fragmentShaderSource = fragmentShaderData.data();
  // Create Fragment Shader Object and get reference
  GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
  // Attach the compiled binary shader
  glShaderBinary(1, &fragmentShader, GL_SHADER_BINARY_FORMAT_SPIR_V_ARB,
                 fragmentShaderSource, fragmentShaderData.size());
  gl_specialize_shader(fragmentShader, "main", 0, 0, 0);

  // Create Shader Program Object and get its reference
  GLuint shaderProgram = glCreateProgram();

  // Attach the Vertex and Fragment Shaders to the Shader program
  glAttachShader(shaderProgram, vertexShader);
  glAttachShader(shaderProgram, fragmentShader);
  // Wrap up/Link all the shaders together into the shader program
  glLinkProgram(shaderProgram);

  // Delete the now unneeded vertex and fragment shader
  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);

  // Create reference container for the Vertex Array Object
  GLuint VAO;
  // Generate the VAO with only one object
  glGenVertexArrays(1, &VAO);
  // Make the VAO the current Vertex Array Object by binding it
  glBindVertexArray(VAO);
  // Configure the Vertex Attribute so that OpenGL know how to read the VBO
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  // Get the 'vp' variable location inside this program
  GLuint attributePosition = glGetAttribLocation(shaderProgram, "vp");
  glVertexAttribPointer(attributePosition, 3, GL_FLOAT, GL_FALSE,
                        3 * sizeof(float), nullptr);
  // Enable the Vertex Attribute so that OpenGL knows to use it
  glEnableVertexAttribArray(attributePosition);

  // Use the program for rendering.
  glUseProgram(shaderProgram);

  // Main while loop
  glutDisplayFunc(display);
  glutMainLoop();

  // Delete all the objects created
  glDeleteVertexArrays(1, &VAO);
  glDeleteBuffers(1, &VBO);
  glDeleteProgram(shaderProgram);

  return EXIT_SUCCESS;
}
