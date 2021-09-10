
// https://www.geeksforgeeks.org/rendering-triangle-using-openglusing-shaders/

#include <GL/glew.h>
#include <GL/glut.h>

#include <cmath>
#include <iostream>

// Vertex Shader source code
const char* vertexShaderSource =
    "#version 410\n"
    "in vec3 vp;\n"
    "void main()\n"
    "{\n"
    "   gl_Position = vec4(vp, 1.0);\n"
    "}\0";

// Fragment Shader source code
const char* fragmentShaderSource =
    "#version 410\n"
    "out vec4 frag_color;\n"
    "void main()\n"
    "{\n"
    "   frag_color = vec4(1.0f, 1.0f, 1.0f, 1.0f);\n"
    "}\n\0";

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

  // Create Vertex Shader Object and get reference
  GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
  // Attach vertex shader source to the Vertex Shader Object
  glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
  // Compile the Vertex Shader into machine code
  glCompileShader(vertexShader);

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
