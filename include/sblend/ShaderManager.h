#ifndef __SHADERMANAGER_H__
#define __SHADERMANAGER_H__
#include "graphics/Shader.h"

namespace sx {

class ShaderManager {
  Shader basicShader;

  Shader modelBasicShader;

  Shader lightningShader;

  Shader modelLightningShader;

  Shader pbrLightningShader;

  Shader modelPbrLightningShader;

  Shader shadowMapShader;

  Shader reflectionShader;

  Shader skyboxShader;

  static ShaderManager *shaderManager;

  ShaderManager();

public:
  static ShaderManager *getShaderManager();

  friend class Application;
};
} // namespace sx

#endif // __SHADERMANAGER_H__