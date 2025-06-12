#include "ShaderManager.h"

namespace sx
{
    Shader ShaderManager::basicShader;
    Shader ShaderManager::modelBasicShader;
    Shader ShaderManager::lightningShader;
    Shader ShaderManager::modelLightningShader;
    Shader ShaderManager::pbrLightningShader;
    Shader ShaderManager::modelPbrLightningShader;
    Shader ShaderManager::shadowMapShader;
    Shader ShaderManager::reflectionShader;
    Shader ShaderManager::skyboxShader;

    ShaderManager *ShaderManager::shaderManager = nullptr;

    ShaderManager::ShaderManager()
    {
        basicShader.autoCompileAndLink("../../shaders/basic.vert", "../../shaders/basic.frag");
        modelBasicShader.autoCompileAndLink("../../shaders/basic.vert", "../../shaders/modelBasic.frag");
        lightningShader.autoCompileAndLink("../../shaders/basic.vert", "../../shaders/lightning.frag");
        modelLightningShader.autoCompileAndLink("../../shaders/basic.vert", "../../shaders/modelLightning.frag");
        pbrLightningShader.autoCompileAndLink("../../shaders/basic.vert", "../../shaders/pbrLightning.frag");
        modelPbrLightningShader.autoCompileAndLink("../../shaders/basic.vert", "../../shaders/modelPbrLightning.frag");
        shadowMapShader.autoCompileAndLink("../../shaders/shadowMap.vert", "../../shaders/shadowMap.frag");
        reflectionShader.autoCompileAndLink("../../shaders/basic.vert", "../../shaders/reflection.frag");
        skyboxShader.autoCompileAndLink("../../shaders/skyBox.vert", "../../shaders/skyBox.frag");
    }

    ShaderManager *ShaderManager::getShaderManager()
    {
        if (shaderManager == nullptr)
            shaderManager = new ShaderManager();

        return shaderManager;
    }

}
