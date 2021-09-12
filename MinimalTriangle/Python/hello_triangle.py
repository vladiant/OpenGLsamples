# https://stackabuse.com/brief-introduction-to-opengl-in-python-with-pyopengl
# pip3 install PyOpenGL PyOpenGL_accelerate
# https://pythonprogramming.net/opengl-rotating-cube-example-pyopengl-tutorial/
# https://gist.github.com/deepankarsharma/3494203

import OpenGL

from OpenGL.GL import *
from OpenGL.GLUT import *
from OpenGL.GLU import *

w, h = 500, 500


def triangle():
    glClearColor(0.5, 0.5, 0.5, 1.0)
    glBegin(GL_TRIANGLES)
    glColor3f(0.0, 0.0, 1.0)
    glVertex2f(w / 2, h / 3)
    glColor3f(0.0, 1.0, 0.0)
    glVertex2f(w / 4, h * 2 / 3)
    glColor3f(1.0, 0.0, 0.0)
    glVertex2f(w * 3 / 4, h * 2 / 3)
    glEnd()


def iterate():
    glViewport(0, 0, w, h)
    glMatrixMode(GL_PROJECTION)
    glLoadIdentity()
    glOrtho(0, w, 0, h, -1, 1)
    glScalef(1, -1, 1)
    glTranslatef(0, -h, 0)


def showScreen():
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT)
    glLoadIdentity()
    iterate()
    triangle()
    glutSwapBuffers()


glutInit()
glutInitDisplayMode(GLUT_RGBA)
glutInitWindowSize(w, h)
glutInitWindowPosition(0, 0)
wind = glutCreateWindow("Single Triangle")
glutDisplayFunc(showScreen)
glutIdleFunc(showScreen)
glutMainLoop()
