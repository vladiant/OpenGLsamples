#include <GL/gl.h>
#include <GL/glx.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static bool ctxErrorOccurred = false;
static int ctxErrorHandler(Display *dpy, XErrorEvent *ev) {
  ctxErrorOccurred = true;
  return 0;
}

int main(int argc, char *argv[]) {
  Display *display = XOpenDisplay(NULL);

  if (!display) {
    printf("Failed to open X display\n");
    exit(1);
  }

  // Get a matching FB config
  constexpr int visual_attribs[] = {
      GLX_X_RENDERABLE, True, GLX_DRAWABLE_TYPE, GLX_WINDOW_BIT,
      GLX_RENDER_TYPE, GLX_RGBA_BIT, GLX_X_VISUAL_TYPE, GLX_TRUE_COLOR,
      GLX_RED_SIZE, 8, GLX_GREEN_SIZE, 8, GLX_BLUE_SIZE, 8, GLX_ALPHA_SIZE, 8,
      GLX_DEPTH_SIZE, 24, GLX_STENCIL_SIZE, 8, GLX_DOUBLEBUFFER, True,
      //  GLX_SAMPLE_BUFFERS,
      //  1,
      //  GLX_SAMPLES,
      //  4,
      None};

  int glx_major, glx_minor;

  // FBConfigs were added in GLX version 1.3.
  if (!glXQueryVersion(display, &glx_major, &glx_minor) ||
      ((glx_major == 1) && (glx_minor < 3)) || (glx_major < 1)) {
    printf("Invalid GLX version\n");
    exit(1);
  }

  printf("Getting matching framebuffer configs\n");
  int fbcount;
  GLXFBConfig *fbc = glXChooseFBConfig(display, DefaultScreen(display),
                                       visual_attribs, &fbcount);
  if (!fbc) {
    printf("Failed to retrieve a framebuffer config\n");
    exit(1);
  }
  printf("Found %d matching FB configs.\n", fbcount);

  // Pick the FB config/visual with the most samples per pixel
  printf("Getting XVisualInfos\n");
  int best_fbc = -1, worst_fbc = -1, best_num_samp = -1, worst_num_samp = 999;

  int i;
  for (i = 0; i < fbcount; ++i) {
    XVisualInfo *vi = glXGetVisualFromFBConfig(display, fbc[i]);
    if (vi) {
      int samp_buf, samples;
      glXGetFBConfigAttrib(display, fbc[i], GLX_SAMPLE_BUFFERS, &samp_buf);
      glXGetFBConfigAttrib(display, fbc[i], GLX_SAMPLES, &samples);

      printf(
          "  Matching fbconfig %d, visual ID 0x%lx: SAMPLE_BUFFERS = %d,"
          " SAMPLES = %d\n",
          i, vi->visualid, samp_buf, samples);

      if (best_fbc < 0 || samp_buf && samples > best_num_samp)
        best_fbc = i, best_num_samp = samples;
      if (worst_fbc < 0 || !samp_buf || samples < worst_num_samp)
        worst_fbc = i, worst_num_samp = samples;
    }
    XFree(vi);
  }

  GLXFBConfig bestFbc = fbc[best_fbc];

  // Be sure to free the FBConfig list allocated by glXChooseFBConfig()
  XFree(fbc);

  // Get a visual
  XVisualInfo *vi = glXGetVisualFromFBConfig(display, bestFbc);
  printf("Chosen visual ID = 0x%lu\n", vi->visualid);

  printf("Creating colormap\n");
  XSetWindowAttributes swa;
  Colormap cmap;
  swa.colormap = cmap = XCreateColormap(
      display, RootWindow(display, vi->screen), vi->visual, AllocNone);
  swa.background_pixmap = None;
  swa.border_pixel = 0;
  swa.event_mask = StructureNotifyMask;

  printf("Creating window\n");
  Window win = XCreateWindow(display, RootWindow(display, vi->screen), 0, 0,
                             100, 100, 0, vi->depth, InputOutput, vi->visual,
                             CWBorderPixel | CWColormap | CWEventMask, &swa);
  if (!win) {
    printf("Failed to create window.\n");
    exit(1);
  }

  // Done with the visual info data
  XFree(vi);

  XStoreName(display, win, "GL 3.0 Window");

  printf("Mapping window\n");
  XMapWindow(display, win);

  // Get the default screen's GLX extension list
  const char *glxExts =
      glXQueryExtensionsString(display, DefaultScreen(display));

  GLXContext ctx = 0;

  // Install an X error handler so the application won't exit if GL 3.0
  // context allocation fails.
  //
  // Note this error handler is global.  All display connections in all threads
  // of a process use the same error handler, so be sure to guard against other
  // threads issuing X commands while this code is running.
  ctxErrorOccurred = false;
  int (*oldHandler)(Display *, XErrorEvent *) =
      XSetErrorHandler(&ctxErrorHandler);

  printf("Creating GLX context\n");
  ctx = glXCreateNewContext(display, bestFbc, GLX_RGBA_TYPE, 0, True);

  // Sync to ensure any errors generated are processed.
  XSync(display, False);

  // Restore the original error handler
  XSetErrorHandler(oldHandler);

  if (ctxErrorOccurred || !ctx) {
    printf("Failed to create an OpenGL context\n");
    exit(1);
  }

  // Verifying that context is a direct context
  if (!glXIsDirect(display, ctx)) {
    printf("Indirect GLX rendering context obtained\n");
  } else {
    printf("Direct GLX rendering context obtained\n");
  }

  printf("Making context current\n");
  glXMakeCurrent(display, win, ctx);

  printf("Draw 1\n");
  glClearColor(0, 0.5, 1, 1);
  glClear(GL_COLOR_BUFFER_BIT);
  glXSwapBuffers(display, win);

  sleep(1);

  printf("Draw 2\n");
  glClearColor(1, 0.5, 0, 1);
  glClear(GL_COLOR_BUFFER_BIT);
  glXSwapBuffers(display, win);

  sleep(1);

  printf("Cleanup\n");
  glXMakeCurrent(display, 0, 0);
  glXDestroyContext(display, ctx);

  XDestroyWindow(display, win);
  XFreeColormap(display, cmap);
  XCloseDisplay(display);

  printf("Done.\n");

  return 0;
}
