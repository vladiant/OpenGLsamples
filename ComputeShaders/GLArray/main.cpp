// https://open.gl/feedback
#include <algorithm>
#include <iostream>

#define GL_GLEXT_PROTOTYPES
#include <EGL/egl.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glx.h>
#include <X11/Xatom.h>
#include <X11/Xlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

namespace {
// Vertex shader
const GLchar *vertexShaderSrc = R"glsl(
    #version 150 core

    in float inValue;
    out float outValue;

    void main()
    {
        outValue = sqrt(inValue);
    }
)glsl";

Display *d_dpy{};
Window d_win{};
GLXContext d_ctx{};
}  // namespace

#define WIN_WIDTH 1
#define WIN_HEIGHT 1

void checkErrors(const std::string &desc) {
  GLenum e = glGetError();
  if (e != GL_NO_ERROR) {
    fprintf(stderr, "OpenGL error in \"%s\": %s (%d)\n", desc.c_str(),
            gluErrorString(e), e);
    exit(20);
  }
}

void initGL() {
  if (!(d_dpy = XOpenDisplay(NULL))) {
    fprintf(stderr, "Couldn't open X11 display\n");
    exit(10);
  }

  EGLDisplay egl_dpy;
  EGLint egl_major, egl_minor;

  egl_dpy = eglGetDisplay(d_dpy);
  if (!egl_dpy) {
    printf("Error: eglGetDisplay() failed\n");
    exit(10);
  }

  if (!eglInitialize(egl_dpy, &egl_major, &egl_minor)) {
    printf("Error: eglInitialize() failed\n");
    exit(10);
  }

  int attr[] = {GLX_RGBA, GLX_RED_SIZE,  1, GLX_GREEN_SIZE,
                1,        GLX_BLUE_SIZE, 1, GLX_DOUBLEBUFFER,
                None};

  int scrnum = DefaultScreen(d_dpy);
  Window root = RootWindow(d_dpy, scrnum);

  int elemc;
  GLXFBConfig *fbcfg = glXChooseFBConfig(d_dpy, scrnum, NULL, &elemc);
  if (!fbcfg) {
    fprintf(stderr, "Couldn't get FB configs\n");
    exit(11);
  }

  XVisualInfo *visinfo = glXChooseVisual(d_dpy, scrnum, attr);

  if (!visinfo) {
    fprintf(stderr, "Couldn't get a visual\n");
    exit(12);
  }

  // Window parameters
  XSetWindowAttributes winattr;
  winattr.background_pixel = 0;
  winattr.border_pixel = 0;
  winattr.colormap = XCreateColormap(d_dpy, root, visinfo->visual, AllocNone);
  winattr.event_mask = StructureNotifyMask | ExposureMask | KeyPressMask;
  unsigned long mask = CWBackPixel | CWBorderPixel | CWColormap | CWEventMask;

  d_win = XCreateWindow(d_dpy, root, -1, -1, WIN_WIDTH, WIN_HEIGHT, 0,
                        visinfo->depth, InputOutput, visinfo->visual, mask,
                        &winattr);

  d_ctx = glXCreateContext(d_dpy, visinfo, d_ctx, true);

  if (!d_ctx) {
    fprintf(stderr, "Couldn't create an OpenGL context\n");
    exit(13);
  }

  XFree(visinfo);

  glXMakeCurrent(d_dpy, d_win, d_ctx);

  checkErrors("Window init");
}

int main(int argc, char *argv[]) {
  // Required
  initGL();

  // Compile shader
  GLuint shader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(shader, 1, &vertexShaderSrc, nullptr);
  glCompileShader(shader);

  // Create program and specify transform feedback variables
  GLuint program = glCreateProgram();
  glAttachShader(program, shader);

  const GLchar *feedbackVaryings[] = {"outValue"};
  glTransformFeedbackVaryings(program, 1, feedbackVaryings,
                              GL_INTERLEAVED_ATTRIBS);

  glLinkProgram(program);
  glUseProgram(program);

  // Create VAO
  GLuint vao;
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);

  // Create input VBO and vertex format
  GLfloat data[] = {1.0f, 2.0f, 3.0f, 4.0f, 5.0f,
                    6.0f, 7.0f, 8.0f, 9.0f, 10.0f};
  constexpr size_t data_size = sizeof(data) / sizeof(data[0]);

  GLuint vbo;
  glGenBuffers(1, &vbo);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(data), data, GL_STATIC_DRAW);

  GLint inputAttrib = glGetAttribLocation(program, "inValue");
  glEnableVertexAttribArray(inputAttrib);
  glVertexAttribPointer(inputAttrib, 1, GL_FLOAT, GL_FALSE, 0, 0);

  // Create transform feedback buffer
  GLuint tbo;
  glGenBuffers(1, &tbo);
  glBindBuffer(GL_ARRAY_BUFFER, tbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(data), nullptr, GL_STATIC_READ);

  // Perform feedback transform
  glEnable(GL_RASTERIZER_DISCARD);

  glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, tbo);

  glBeginTransformFeedback(GL_POINTS);
  glDrawArrays(GL_POINTS, 0, data_size);
  glEndTransformFeedback();

  glDisable(GL_RASTERIZER_DISCARD);

  glFlush();

  // Fetch and print results
  GLfloat feedback[data_size]{};
  glGetBufferSubData(GL_TRANSFORM_FEEDBACK_BUFFER, 0, sizeof(feedback),
                     feedback);

  std::for_each(std::begin(feedback), std::end(feedback),
                [](GLfloat value) { std::cout << value << std::endl; });

  glDeleteProgram(program);
  glDeleteShader(shader);

  glDeleteBuffers(1, &tbo);
  glDeleteBuffers(1, &vbo);

  glDeleteVertexArrays(1, &vao);
  return 0;
}
