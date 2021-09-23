// https://github.com/guoyejun/gles32_compute_shader_basic/blob/master/gl3_cs_basic.cpp
// g++ -Wall gl3_cs_basic.cpp -lGL -lEGL -o gl3_cs_basic.x

#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <GLES3/gl32.h>

static const char gComputeShader[] =
    "#version 320 es\n"
    "layout(local_size_x = 8) in;\n"
    "layout(binding = 0) readonly buffer Input0 {\n"
    "    float data[];\n"
    "} input0;\n"
    "layout(binding = 1) readonly buffer Input1 {\n"
    "    float data[];\n"
    "} input1;\n"
    "layout(binding = 2) writeonly buffer Output {\n"
    "    float data[];\n"
    "} output0;\n"
    "void main()\n"
    "{\n"
    "    uint idx = gl_GlobalInvocationID.x;\n"
    "    float f = input0.data[idx] + input1.data[idx];"
    "    output0.data[idx] = f;\n"
    "}\n";

void CHECK() {
  const GLenum err = glGetError();
  if (err != GL_NO_ERROR) {
    printf("glGetError returns %d\n", err);
  }
}

GLuint loadShader(GLenum shaderType, const char* pSource) {
  GLuint shader = glCreateShader(shaderType);
  if (shader) {
    glShaderSource(shader, 1, &pSource, NULL);
    glCompileShader(shader);
    GLint compiled = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
    if (!compiled) {
      GLint infoLen = 0;
      glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);
      if (infoLen) {
        char* buf = (char*)malloc(infoLen);
        if (buf) {
          glGetShaderInfoLog(shader, infoLen, NULL, buf);
          fprintf(stderr, "Could not compile shader %d:\n%s\n", shaderType,
                  buf);
          free(buf);
        }
        glDeleteShader(shader);
        shader = 0;
      }
    }
  }
  return shader;
}

GLuint createComputeProgram(const char* pComputeSource) {
  GLuint computeShader = loadShader(GL_COMPUTE_SHADER, pComputeSource);
  if (!computeShader) {
    return 0;
  }

  GLuint program = glCreateProgram();
  if (program) {
    glAttachShader(program, computeShader);
    glLinkProgram(program);
    GLint linkStatus = GL_FALSE;
    glGetProgramiv(program, GL_LINK_STATUS, &linkStatus);
    if (linkStatus != GL_TRUE) {
      GLint bufLength = 0;
      glGetProgramiv(program, GL_INFO_LOG_LENGTH, &bufLength);
      if (bufLength) {
        char* buf = (char*)malloc(bufLength);
        if (buf) {
          glGetProgramInfoLog(program, bufLength, NULL, buf);
          fprintf(stderr, "Could not link program:\n%s\n", buf);
          free(buf);
        }
      }
      glDeleteProgram(program);
      program = 0;
    }
  }
  return program;
}

void setupSSBufferObject(GLuint& ssbo, GLuint index, float* pIn, GLuint count) {
  glGenBuffers(1, &ssbo);
  glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);

  glBufferData(GL_SHADER_STORAGE_BUFFER, count * sizeof(float), pIn,
               GL_STATIC_DRAW);
  glBindBufferBase(GL_SHADER_STORAGE_BUFFER, index, ssbo);
}

void tryComputeShader() {
  GLuint computeProgram;
  GLuint input0SSbo;
  GLuint input1SSbo;
  GLuint outputSSbo;

  CHECK();
  computeProgram = createComputeProgram(gComputeShader);
  CHECK();

  const GLuint arraySize = 8000;
  float f0[arraySize];
  float f1[arraySize];
  for (GLuint i = 0; i < arraySize; ++i) {
    f0[i] = i;
    f1[i] = i;
  }
  setupSSBufferObject(input0SSbo, 0, f0, arraySize);
  setupSSBufferObject(input1SSbo, 1, f1, arraySize);
  setupSSBufferObject(outputSSbo, 2, NULL, arraySize);
  CHECK();

  glUseProgram(computeProgram);
  glDispatchCompute(1000, 1, 1);  // arraySize/local_size_x
  glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
  CHECK();

  glBindBuffer(GL_SHADER_STORAGE_BUFFER, outputSSbo);
  float* pOut = (float*)glMapBufferRange(
      GL_SHADER_STORAGE_BUFFER, 0, arraySize * sizeof(float), GL_MAP_READ_BIT);
  for (GLuint i = 0; i < arraySize; ++i) {
    if (fabs(pOut[i] - (f0[i] + f1[i])) > 0.0001) {
      printf(
          "verification FAILED at array index %d, actual: %f, expected: %f\n",
          i, pOut[i], f0[i] + f1[i]);
      glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
      return;
    }
  }
  glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
  printf("verification PASSED\n");
  glDeleteProgram(computeProgram);
}

int main(int /*argc*/, char** /*argv*/) {
  EGLDisplay dpy = eglGetDisplay(EGL_DEFAULT_DISPLAY);
  if (dpy == EGL_NO_DISPLAY) {
    printf("eglGetDisplay returned EGL_NO_DISPLAY.\n");
    return 0;
  }

  EGLint majorVersion;
  EGLint minorVersion;
  EGLBoolean returnValue = eglInitialize(dpy, &majorVersion, &minorVersion);
  if (returnValue != EGL_TRUE) {
    printf("eglInitialize failed\n");
    return 0;
  }

  EGLConfig cfg;
  EGLint count;
  EGLint s_configAttribs[] = {EGL_RENDERABLE_TYPE, EGL_OPENGL_ES3_BIT_KHR,
                              EGL_NONE};
  if (eglChooseConfig(dpy, s_configAttribs, &cfg, 1, &count) == EGL_FALSE) {
    printf("eglChooseConfig failed\n");
    return 0;
  }

  EGLint context_attribs[] = {EGL_CONTEXT_CLIENT_VERSION, 3, EGL_NONE};
  EGLContext context =
      eglCreateContext(dpy, cfg, EGL_NO_CONTEXT, context_attribs);
  if (context == EGL_NO_CONTEXT) {
    printf("eglCreateContext failed\n");
    return 0;
  }
  returnValue = eglMakeCurrent(dpy, EGL_NO_SURFACE, EGL_NO_SURFACE, context);
  if (returnValue != EGL_TRUE) {
    printf("eglMakeCurrent failed returned %d\n", returnValue);
    return 0;
  }

  {
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
    // END
  }

  tryComputeShader();

  eglDestroyContext(dpy, context);
  eglTerminate(dpy);

  return 0;
}
