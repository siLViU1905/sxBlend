#include "ShaderManager.h"

namespace sx {

ShaderManager *ShaderManager::shaderManager = nullptr;

ShaderManager::ShaderManager() {
  basicShader.autoCompileAndLink("../../shaders/basic.vert",
                                 "../../shaders/basic.frag");
  modelBasicShader.autoCompileAndLink("../../shaders/basic.vert",
                                      "../../shaders/modelBasic.frag");
  lightningShader.autoCompileAndLink("../../shaders/basic.vert",
                                     "../../shaders/lightning.frag");
  modelLightningShader.autoCompileAndLink("../../shaders/basic.vert",
                                          "../../shaders/modelLightning.frag");
  pbrLightningShader.autoCompileAndLink("../../shaders/basic.vert",
                                        "../../shaders/pbrLightning.frag");
  modelPbrLightningShader.autoCompileAndLink(
      "../../shaders/basic.vert", "../../shaders/modelPbrLightning.frag");
  shadowMapShader.autoCompileAndLink("../../shaders/shadowMap.vert",
                                     "../../shaders/shadowMap.frag");
  reflectionShader.autoCompileAndLink("../../shaders/basic.vert",
                                      "../../shaders/reflection.frag");
  skyboxShader.autoCompileAndLink("../../shaders/skyBox.vert",
                                  "../../shaders/skyBox.frag");
  terrainBasicShader.autoCompileAndLink("../../shaders/basic.vert",
                                        "../../shaders/terrainBasic.frag");
  terrainLightningShader.autoCompileAndLink("../../shaders/basic.vert", "../../shaders/terrainLightning.frag");
  terrainPbrLightningShader.autoCompileAndLink("../../shaders/basic.vert", "../../shaders/terrainPbrLightning.frag");
}

ShaderManager *ShaderManager::getShaderManager() {
  if (shaderManager == nullptr)
    shaderManager = new ShaderManager();

  return shaderManager;
}

} // namespace sx
