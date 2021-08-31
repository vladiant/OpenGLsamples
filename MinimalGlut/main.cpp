#include <GL/glut.h>

// g++ main.c -lglut -lGL

void reshape(int w, int h) {
  glViewport(0, 0, w, h);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0, w, 0, h, -1, 1);
  glScalef(1, -1, 1);
  glTranslatef(0, -h, 0);
}

void display(void) {
  glClear(GL_COLOR_BUFFER_BIT);
  glBegin(GL_TRIANGLES);
  glColor3f(0.0, 0.0, 1.0);
  glVertex2i(110, 20);
  glColor3f(0.0, 1.0, 0.0);
  glVertex2i(200, 200);
  glColor3f(1.0, 0.0, 0.0);
  glVertex2i(20, 200);
  glEnd();
  glFlush();
}

int main(int argc, char** argv) {
  glutInit(&argc, argv);
  glutCreateWindow("single triangle");
  glutDisplayFunc(display);
  glutReshapeFunc(reshape);
  glutMainLoop();
  return 0;
}
