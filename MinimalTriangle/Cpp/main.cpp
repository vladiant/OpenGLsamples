#include <GL/glut.h>

constexpr int kWidth = 500;
constexpr int kHeight = 500;

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
  glClearColor(0.5, 0.5, 0.5, 1.0);
  glBegin(GL_TRIANGLES);
  glColor3f(0.0, 0.0, 1.0);
  glVertex2i(kWidth / 2.0, kHeight / 3.0);
  glColor3f(0.0, 1.0, 0.0);
  glVertex2f(kWidth / 4.0, kHeight * 2.0 / 3.0);
  glColor3f(1.0, 0.0, 0.0);
  glVertex2f(kWidth * 3.0 / 4.0, kHeight * 2.0 / 3.0);
  glEnd();
  glFlush();
}

int main(int argc, char** argv) {
  glutInit(&argc, argv);
  glutInitWindowSize(kWidth, kHeight);
  glutInitWindowPosition(0, 0);
  glutCreateWindow("single triangle");
  glutDisplayFunc(display);
  glutReshapeFunc(reshape);
  glutMainLoop();
  return 0;
}
