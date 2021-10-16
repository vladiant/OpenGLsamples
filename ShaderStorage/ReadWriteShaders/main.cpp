
// https://www.geeksforgeeks.org/rendering-triangle-using-openglusing-shaders/

#include <GL/glew.h>
#include <GL/glut.h>

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

  result.push_back('\0');

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

int main(int argc, char** argv) {
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
  glutInitWindowSize(500, 500);
  glutInitWindowPosition(100, 50);
  glutCreateWindow("Triangle");

  glewInit();

  // Check program formats
  {
    GLint formats = 0;
    glGetIntegerv(GL_NUM_PROGRAM_BINARY_FORMATS, &formats);
    if (formats < 1) {
      std::cerr << "Driver does not support any program binary formats."
                << std::endl;
      return EXIT_FAILURE;
    }

    std::cout << "Supported program binary formats count: " << formats << '\n';

    std::vector<GLint> formatTypes(formats);
    glGetIntegerv(GL_PROGRAM_BINARY_FORMATS, formatTypes.data());
    for (auto formatType : formatTypes) {
      std::cout << "Program format type: " << formatType << '\n';
    }
  }

  // Check shader formats
  {
    GLint formats = 0;
    glGetIntegerv(GL_NUM_SHADER_BINARY_FORMATS, &formats);
    if (formats < 1) {
      std::cerr << "Driver does not support any shader binary formats."
                << std::endl;
      return EXIT_FAILURE;
    }

    std::cout << "Supported shader binary formats count: " << formats << '\n';

    std::vector<GLint> formatTypes(formats);
    glGetIntegerv(GL_SHADER_BINARY_FORMATS, formatTypes.data());
    for (auto formatType : formatTypes) {
      std::cout << "Shader format type: " << formatType << '\n';
    }
  }

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
  const auto vertexShaderData = readFile("shaders/simple_shader.vert");
  const auto vertexShaderSource = vertexShaderData.data();
  // Create Vertex Shader Object and get reference
  GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
  // Attach vertex shader source to the Vertex Shader Object
  glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
  // Compile the Vertex Shader into machine code
  glCompileShader(vertexShader);

  // Read Vertex Shader source code
  const auto fragmentShaderData = readFile("shaders/simple_shader.frag");
  const auto fragmentShaderSource = fragmentShaderData.data();
  // Create Fragment Shader Object and get reference
  GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
  // Attach fragment shader source to the Fragment Shader Object
  glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
  // Compile the Fragment Shader into machine code
  glCompileShader(fragmentShader);

  // Create Shader Program Object and get its reference
  GLuint shaderProgram = glCreateProgram();

  // Attach the Vertex and Fragment Shaders to the Shader program
  glAttachShader(shaderProgram, vertexShader);
  glAttachShader(shaderProgram, fragmentShader);
  // Wrap up/Link all the shaders together into the shader program
  glLinkProgram(shaderProgram);

  // Common I/O params
  GLenum format = 0;
  std::string fName{"program_shader.bin"};

  // Write shader program
  {
    GLint length = 0;
    glGetProgramiv(shaderProgram, GL_PROGRAM_BINARY_LENGTH, &length);
    std::cout << "Shader program binary length: " << length << '\n';

    // Retrieve the binary code
    std::vector<GLubyte> buffer(length);

    glGetProgramBinary(shaderProgram, length, nullptr, &format, buffer.data());

    // Write the binary to a file
    std::cout << "Writing to " << fName << " , binary format = " << format
              << '\n';
    std::ofstream out(fName, std::ios::binary);
    out.write(reinterpret_cast<char*>(buffer.data()), length);
    out.close();
  }

  // Cleanup before reading
  glDeleteProgram(shaderProgram);
  shaderProgram = glCreateProgram();

  // Read shader program
  {
    // Load binary from file
    std::ifstream inputStream(fName, std::ios::binary);
    std::istreambuf_iterator<char> startIt(inputStream), endIt;
    std::vector<char> buffer(startIt, endIt);
    inputStream.close();

    // Install shader binary
    glProgramBinary(shaderProgram, format, buffer.data(), buffer.size());

    // Check for success/failure
    GLint status;
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &status);
    if (GL_FALSE == status) {
      std::cerr << "Error linking loaded program!\n";
      return EXIT_FAILURE;
    }

    std::cout << "Program successfully loaded\n";
  }

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
