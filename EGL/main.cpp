// https://www.khronos.org/registry/EGL/sdk/docs/man/html/eglIntro.xhtml
// https://stackoverflow.com/questions/9196526/how-to-create-a-window-and-fill-it-with-color-using-openes-2-0-x11

#include <EGL/egl.h>
#include <GL/gl.h>
//#include <GLES2/gl2.h>
#include <X11/Xatom.h>
#include <X11/Xlib.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

NativeWindowType createNativeWindow(void) {
  NativeWindowType native_window;

  Window root;
  XSetWindowAttributes swa;
  XSetWindowAttributes xattr;
  Atom wm_state;
  XWMHints hints;
  XEvent xev;
  EGLConfig ecfg;
  Display* x_display;

  // X11 native display initialization
  x_display = XOpenDisplay(NULL);
  if (x_display == NULL) {
    return EGL_FALSE;
  }

  root = DefaultRootWindow(x_display);

  swa.event_mask = ExposureMask | PointerMotionMask | KeyPressMask;
  native_window =
      XCreateWindow(x_display, root, 0, 0, 640, 480, 0, CopyFromParent,
                    InputOutput, CopyFromParent, CWEventMask, &swa);

  xattr.override_redirect = false;
  XChangeWindowAttributes(x_display, native_window, CWOverrideRedirect, &xattr);

  hints.input = true;
  hints.flags = InputHint;
  XSetWMHints(x_display, native_window, &hints);

  // make the window visible on the screen
  XMapWindow(x_display, native_window);
  XStoreName(x_display, native_window, "Test");

  // get identifiers for the provided atom name strings
  wm_state = XInternAtom(x_display, "_NET_WM_STATE", false);

  memset(&xev, 0, sizeof(xev));
  xev.type = ClientMessage;
  xev.xclient.window = native_window;
  xev.xclient.message_type = wm_state;
  xev.xclient.format = 32;
  xev.xclient.data.l[0] = 1;
  xev.xclient.data.l[1] = false;
  XSendEvent(x_display, DefaultRootWindow(x_display), false,
             SubstructureNotifyMask, &xev);

  return native_window;
}

static EGLint const attribute_list[] = {EGL_RED_SIZE,  1, EGL_GREEN_SIZE, 1,
                                        EGL_BLUE_SIZE, 1, EGL_NONE};

int main(int argc, char** argv) {
  EGLDisplay display;
  EGLConfig config;
  EGLContext context;
  EGLSurface surface;
  NativeWindowType native_window;
  EGLint num_config;

  // get an EGL display connection
  display = eglGetDisplay(EGL_DEFAULT_DISPLAY);

  // initialize the EGL display connection
  eglInitialize(display, NULL, NULL);

  // get an appropriate EGL frame buffer configuration
  eglChooseConfig(display, attribute_list, &config, 1, &num_config);

  // create an EGL rendering context
  context = eglCreateContext(display, config, EGL_NO_CONTEXT, NULL);

  // create a native window
  native_window = createNativeWindow();

  // create an EGL window surface
  surface = eglCreateWindowSurface(display, config, native_window, NULL);

  // connect the context to the surface
  eglMakeCurrent(display, surface, surface, context);

  // clear the color buffer */
  glClearColor(1.0, 1.0, 0.0, 1.0);
  glClear(GL_COLOR_BUFFER_BIT);
  glFlush();

  eglSwapBuffers(display, surface);

  sleep(2);
  return EXIT_SUCCESS;
}
