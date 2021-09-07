// Hello World with OpenGL compute shader!
// I have wanted to do this for a long time and now it is done!
// This is slightly less elegant than its siblings since compute shaders
// can not deal with bytes. I chose to repackage to int on the CPU.
// This is an example, use freely.
// By Ingemar Ragnemalm 2017

// Like my similar demos, if it outputs "Hello World!" then it works correctly.
// If it fails, it will most likely output "Hello Hello"

// This is a stand-alone Linux variant, includnig a somewhat simplified context
// creation based on MicroGlut, using an invisible window.

// Compile like this:

// gcc -o hello hello-cs-standalone-linux.c -lGL -lX11

#define GL_GLEXT_PROTOTYPES

#include <GL/gl.h>
#include <GL/glext.h>
#include <GL/glx.h>
#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/times.h>
#include <unistd.h>

Display *dpy = nullptr;
char *dpyName = nullptr;

// ----------------- Stripped down context creation ------------------------

static void make_window(const char *name) {
  int scrnum;
  XSetWindowAttributes attr;
  unsigned long mask;
  Window root;
  Window win;
  GLXContext ctx;
  XVisualInfo *visinfo;

  dpy = XOpenDisplay(nullptr);
  if (!dpy) {
    printf("Error: couldn't open display %s\n",
           name ? name : getenv("DISPLAY"));
  }

  scrnum = DefaultScreen(dpy);
  root = RootWindow(dpy, scrnum);

  // Old-style context creation. Seems to work just fine for this!

  int attribs[] = {GLX_RGBA, None};

  visinfo = glXChooseVisual(dpy, scrnum, attribs);
  if (!visinfo) {
    printf("Error: couldn't get a visual according to settings\n");
    exit(1);
  }

  ctx = glXCreateContext(dpy, visinfo, 0, True);
  if (ctx == nullptr) printf("No ctx!\n");

  // window attributes
  attr.background_pixel = 0;
  attr.border_pixel = 0;
  attr.colormap = XCreateColormap(dpy, root, visinfo->visual, AllocNone);
  attr.event_mask = 0;  //  StructureNotifyMask | ExposureMask | KeyPressMask |
                        //  KeyReleaseMask | ButtonPress | ButtonReleaseMask |
                        //  Button1MotionMask | PointerMotionMask;
  attr.override_redirect = 0;
  mask = CWBackPixel | CWBorderPixel | CWColormap | CWEventMask |
         CWOverrideRedirect;

  win = XCreateWindow(dpy, root, 50, 50, 50, 50, 0, visinfo->depth, InputOutput,
                      visinfo->visual, mask, &attr);

  if (!ctx) {
    printf("Error: glXCreateContext failed\n");
    exit(1);
  }

  XFree(visinfo);

  glXMakeCurrent(dpy, win, ctx);

  // print some compute limits (not strictly necessary)
  GLint work_group_count[3] = {0};
  for (unsigned i = 0; i < 3; i++)
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, i, &work_group_count[i]);
  printf("GL_MAX_COMPUTE_WORK_GROUP_COUNT: %d, %d, %d\n", work_group_count[0],
         work_group_count[1], work_group_count[2]);

  GLint work_group_size[3] = {0};
  for (unsigned i = 0; i < 3; i++)
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, i, &work_group_size[i]);
  printf("GL_MAX_COMPUTE_WORK_GROUP_SIZE: %d, %d, %d\n", work_group_size[0],
         work_group_size[1], work_group_size[2]);

  GLint max_invocations;
  glGetIntegerv(GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS, &max_invocations);
  printf("GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS: %d\n", max_invocations);

  GLint mem_size = 0;
  glGetIntegerv(GL_MAX_COMPUTE_SHARED_MEMORY_SIZE, &mem_size);
  printf("GL_MAX_COMPUTE_SHARED_MEMORY_SIZE: %d\n", mem_size);
}

// ----------------- Functions for reading the shader file and compiling it
// ------------------------

char *readFile(char *file) {
  FILE *fptr;
  long length;
  char *buf;

  fptr = fopen(file, "rb");  // Open file for reading
  if (!fptr)                 // Return nullptr on failure
    return nullptr;
  fseek(fptr, 0, SEEK_END);  // Seek to the end of the file
  length = ftell(fptr);      // Find out how many bytes into the file we are
  buf = (char *)malloc(length + 1);  // Allocate a buffer for the entire length
                                     // of the file and a null terminator
  fseek(fptr, 0, SEEK_SET);          // Go back to the beginning of the file
  fread(buf, length, 1,
        fptr);      // Read the contents of the file in to the buffer
  fclose(fptr);     // Close the file
  buf[length] = 0;  // Null terminator

  return buf;  // Return the buffer
}

// Infolog: Show result of shader compilation
void printShaderInfoLog(GLuint obj, const char *fn) {
  GLint infologLength = 0;
  GLint charsWritten = 0;
  char *infoLog;

  glGetShaderiv(obj, GL_INFO_LOG_LENGTH, &infologLength);

  if (infologLength > 2) {
    fprintf(stderr, "[From %s:]\n", fn);
    infoLog = (char *)malloc(infologLength);
    glGetShaderInfoLog(obj, infologLength, &charsWritten, infoLog);
    fprintf(stderr, "%s\n", infoLog);
    free(infoLog);
  }
}

void printProgramInfoLog(GLuint obj, const char *vfn) {
  GLint infologLength = 0;
  GLint charsWritten = 0;
  char *infoLog;

  glGetProgramiv(obj, GL_INFO_LOG_LENGTH, &infologLength);

  if (infologLength > 2) {
    fprintf(stderr, "[From %s:]\n", vfn);
    infoLog = (char *)malloc(infologLength);
    glGetProgramInfoLog(obj, infologLength, &charsWritten, infoLog);
    fprintf(stderr, "%s\n", infoLog);
    free(infoLog);
  }
}

// Compile a shader, return reference to it
GLuint compileShaders(const char *vs, const char *vfn) {
  GLuint v, p;

  v = glCreateShader(
      GL_COMPUTE_SHADER);  // GL_COMPUTE_SHADER_EXT? GL_COMPUTE_SHADER_ARB?
  glShaderSource(v, 1, &vs, nullptr);
  glCompileShader(v);

  p = glCreateProgram();  //#include "opengl.h"

  glAttachShader(p, v);
  glLinkProgram(p);
  glUseProgram(p);

  printShaderInfoLog(v, vfn);
  printProgramInfoLog(p, vfn);

  return p;
}

GLuint loadShader(char *filename) {
  char *vs;

  vs = readFile(filename);

  return compileShaders(vs, filename);
}

int main(int argc, char **argv) {
  // create a GL context
  make_window("TEST1");

  // Load and compile the compute shader
  char fileName[] = "hello.cs";
  GLuint p = loadShader(fileName);

  GLuint ssbo, ssbo2;  // Shader Storage Buffer Object

  // Some data
  constexpr size_t N = 16;
  char a[N] = "Hello \0\0\0\0\0\0";
  int ac[N];
  int b[N] = {15, 10, 6, 0, -11, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  int *ptr;
  int i;

  printf("%s", a);

  // PROBLEM: No bytes in shaders!
  // I chose to package to int on the CPU.
  // Convert string to int:
  for (i = 0; i < N; i++) ac[i] = a[i];

  // Create buffer, upload data
  glGenBuffers(1, &ssbo);
  glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
  glBufferData(GL_SHADER_STORAGE_BUFFER, 16 * sizeof(int), &ac, GL_STATIC_DRAW);

  // Tell it where the input goes!
  // The "5" matches a "layuot" number in the shader.
  // (Can we ask the shader about the number? I must try that.)
  glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 5, ssbo);  // binding = 5

  // Same for the other buffer, offsets, ID 6
  glGenBuffers(1, &ssbo2);
  glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo2);
  glBufferData(GL_SHADER_STORAGE_BUFFER, 16 * sizeof(int), &b, GL_STATIC_DRAW);
  glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 6, ssbo2);  // binding = 6

  // Get rolling!
  glDispatchCompute(1, 1, 1);  // Work groups launch

  // Get data back!
  glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
  ptr = (int *)glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_READ_ONLY);
  // Convert int to string:
  for (i = 0; i < 16; i++) {
    a[i] = ptr[i];
  }
  printf("%s\n", a);
}
