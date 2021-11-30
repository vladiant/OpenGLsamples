/* ============================================================================
**
** Demonstration of rendering to texture
** Copyright (C) 2005  Julien Guertault
**
** This program is free software; you can redistribute it and/or
** modify it under the terms of the GNU General Public License
** as published by the Free Software Foundation; either version 2
** of the License, or (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
**
** ========================================================================= */

#include <stdlib.h>
#include <GL/glut.h>
#include <math.h>

#define		SIZE	256

static unsigned char texture[3 * SIZE * SIZE];
static unsigned int texture_id;
static int window_width = 500;
static int window_height = 500;


/*
** Just a textured cube
*/
void		Cube(void)
{
  glBegin(GL_QUADS);

  glTexCoord2i(0, 0); glVertex3f(-1, -1, -1);
  glTexCoord2i(0, 1); glVertex3f(-1, -1,  1);
  glTexCoord2i(1, 1); glVertex3f(-1,  1,  1);
  glTexCoord2i(1, 0); glVertex3f(-1,  1, -1);

  glTexCoord2i(0, 0); glVertex3f( 1, -1, -1);
  glTexCoord2i(0, 1); glVertex3f( 1, -1,  1);
  glTexCoord2i(1, 1); glVertex3f( 1,  1,  1);
  glTexCoord2i(1, 0); glVertex3f( 1,  1, -1);

  glTexCoord2i(0, 0); glVertex3f(-1, -1, -1);
  glTexCoord2i(0, 1); glVertex3f(-1, -1,  1);
  glTexCoord2i(1, 1); glVertex3f( 1, -1,  1);
  glTexCoord2i(1, 0); glVertex3f( 1, -1, -1);

  glTexCoord2i(0, 0); glVertex3f(-1,  1, -1);
  glTexCoord2i(0, 1); glVertex3f(-1,  1,  1);
  glTexCoord2i(1, 1); glVertex3f( 1,  1,  1);
  glTexCoord2i(1, 0); glVertex3f( 1,  1, -1);

  glTexCoord2i(0, 0); glVertex3f(-1, -1, -1);
  glTexCoord2i(0, 1); glVertex3f(-1,  1, -1);
  glTexCoord2i(1, 1); glVertex3f( 1,  1, -1);
  glTexCoord2i(1, 0); glVertex3f( 1, -1, -1);

  glTexCoord2i(0, 0); glVertex3f(-1, -1,  1);
  glTexCoord2i(0, 1); glVertex3f(-1,  1,  1);
  glTexCoord2i(1, 1); glVertex3f( 1,  1,  1);
  glTexCoord2i(1, 0); glVertex3f( 1, -1,  1);

  glEnd();
}

/*
** Function called to update rendering
*/
void		DisplayFunc(void)
{
  static float alpha = 20;

  glLoadIdentity();
  glTranslatef(0, 0, -10);
  glRotatef(30, 1, 0, 0);
  glRotatef(alpha, 0, 1, 0);

  /* Define a view-port adapted to the texture */
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(20, 1, 5, 15);
  glViewport(0, 0, SIZE, SIZE);
  glMatrixMode(GL_MODELVIEW);

  /* Render to buffer */
  glClearColor(1, 1, 1, 0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  Cube();
  glFlush();

  /* Copy buffer to texture */
  glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 5, 5, 0, 0, SIZE - 10, SIZE - 10);

  /* Render to screen */
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(20, window_width / (float) window_height, 5, 15);
  glViewport(0, 0, window_width, window_height);
  glMatrixMode(GL_MODELVIEW);
  glClearColor(0, 0, 0, 0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  Cube();

  /* End */
  glFlush();
  glutSwapBuffers();

  /* Update again and again */
  alpha = alpha + 0.1;
  glutPostRedisplay();
}

/*
** Function called when the window is created or resized
*/
void		ReshapeFunc(int width, int height)
{
  window_width = width;
  window_height = height;
  glutPostRedisplay();
}

/*
** Function called when a key is hit
*/
void		KeyboardFunc(unsigned char key, int x, int y)
{
  int foo;

  foo = x + y; /* Has no effect: just to avoid a warning */
  if ('q' == key || 'Q' == key || 27 == key)
      exit(0);
}


int		main(int argc, char **argv)
{
  /* Creation of the window */
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
  glutInitWindowSize(500, 500);
  glutCreateWindow("Render to texture");

  /* OpenGL settings */
  glEnable(GL_DEPTH_TEST);

  /* Texture setting */
  glEnable(GL_TEXTURE_2D);
  glGenTextures(1, &texture_id);
  glBindTexture(GL_TEXTURE_2D, texture_id);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SIZE, SIZE, 0, GL_RGB,
	       GL_UNSIGNED_BYTE, texture);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

  /* Declaration of the callbacks */
  glutDisplayFunc(&DisplayFunc);
  glutReshapeFunc(&ReshapeFunc);
  glutKeyboardFunc(&KeyboardFunc);

  /* Loop */
  glutMainLoop();

  /* Never reached */
  return 0;
}

/* ========================================================================= */
