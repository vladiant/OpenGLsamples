#pragma once

// https://lxjk.github.io/2020/03/10/Translate-GLSL-to-SPIRV-for-Vulkan-at-Runtime.html

#include <glslang/SPIRV/GlslangToSpv.h>

struct SpirvHelper {
  static void Init();

  static void Finalize();

  static void InitResources(TBuiltInResource &Resources);

  static bool GLSLtoSPV(EShLanguage stage, const char *pshader,
                        std::vector<unsigned int> &spirv);
};