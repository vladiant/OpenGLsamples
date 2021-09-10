# https://codeloop.org/python-modern-opengl-drawing-triangle/

from OpenGL.GLUT import *
from OpenGL.GL import *
from OpenGL.GL import shaders
import numpy as np

VERTEX_SHADER = """
#version 410
in vec4 vp;
void main() {
    gl_Position = vp; 
}
"""


FRAGMENT_SHADER = """
#version 410
out vec4 frag_color;
void main() {
    frag_color = vec4(1, 1, 1, 1);
}
"""

shaderProgram = None


def initliaze():
    global shaderProgram

    vertexshader = shaders.compileShader(VERTEX_SHADER, GL_VERTEX_SHADER)
    fragmentshader = shaders.compileShader(FRAGMENT_SHADER, GL_FRAGMENT_SHADER)

    shaderProgram = shaders.compileProgram(vertexshader, fragmentshader)

    triangles = [
        # top
        0.0,
        0.5,
        0.0,
        # left
        -0.5,
        -0.5,
        0.0,
        # right
        0.5,
        -0.5,
        0.0,
    ]

    triangles = np.array(triangles, dtype=np.float32)

    VBO = glGenBuffers(1)
    glBindBuffer(GL_ARRAY_BUFFER, VBO)
    glBufferData(GL_ARRAY_BUFFER, triangles.nbytes, triangles, GL_STATIC_DRAW)

    position = glGetAttribLocation(shaderProgram, "vp")
    glVertexAttribPointer(position, 3, GL_FLOAT, GL_FALSE, 0, None)
    glEnableVertexAttribArray(position)


def render():
    global shaderProgram
    glClearColor(0, 0, 0, 1)
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT)

    glUseProgram(shaderProgram)

    glDrawArrays(GL_TRIANGLES, 0, 3)

    glUseProgram(0)
    glutSwapBuffers()


def main():

    glutInit([])
    glutInitWindowSize(640, 480)
    glutCreateWindow("pyopengl with glut")
    initliaze()
    glutDisplayFunc(render)
    glutMainLoop()


if __name__ == "__main__":
    main()
