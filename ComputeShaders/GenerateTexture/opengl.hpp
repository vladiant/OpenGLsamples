#pragma once
#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glx.h>
#include <X11/Xlib.h>

// This includes the new stuff, supplied by the application
#include <GL/glext.h>

#include <string>

#define WIN_WIDTH 800
#define WIN_HEIGHT 600

#define GLX_CONTEXT_MAJOR_VERSION_ARB 0x2091
#define GLX_CONTEXT_MINOR_VERSION_ARB 0x2092
typedef GLXContext (*glXCreateContextAttribsARBProc)(Display*, GLXFBConfig,
                                                     GLXContext, Bool,
                                                     const int*);

void initGL();
void swapBuffers();

// Return handles
GLuint genTexture();
GLuint genRenderProg(GLuint);  // Texture as the param
GLuint genComputeProg(GLuint);

void checkErrors(std::string);
