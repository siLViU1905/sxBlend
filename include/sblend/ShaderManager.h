#ifndef __SHADERMANAGER_H__
#define __SHADERMANAGER_H__
#include "graphics/Shader.h"

namespace sx
{

    class ShaderManager
    {
        static Shader basicShader;

        static Shader modelBasicShader;

        static Shader lightningShader;

        static Shader modelLightningShader;

        static Shader pbrLightningShader;

        static Shader modelPbrLightningShader;

        static Shader shadowMapShader;

        static Shader reflectionShader;

        static ShaderManager* shaderManager;

        ShaderManager();

        public:
        static ShaderManager* getShaderManager();

        friend class Application;
    };
}

#endif // __SHADERMANAGER_H__