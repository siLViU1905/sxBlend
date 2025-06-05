#include "ShaderManager.h"

namespace sx
{
    Shader ShaderManager::basicShader;
    Shader ShaderManager::lightningShader;
    Shader ShaderManager::pbrLightningShader;
    Shader ShaderManager::shadowMapShader;

    ShaderManager *ShaderManager::shaderManager = nullptr;

    ShaderManager::ShaderManager()
    {
        basicShader.autoCompileAndLink("../../shaders/basic.vert", "../../shaders/basic.frag");
        lightningShader.autoCompileAndLink("../../shaders/basic.vert", "../../shaders/lightning.frag");
        pbrLightningShader.autoCompileAndLink("../../shaders/basic.vert", "../../shaders/pbrLightning.frag");
        shadowMapShader.autoCompileAndLink("../../shaders/shadowMap.vert", "../../shaders/shadowMap.frag");
    }

    ShaderManager *ShaderManager::getShaderManager()
    {
        if (shaderManager == nullptr)
            shaderManager = new ShaderManager();

        return shaderManager;
    }

}
