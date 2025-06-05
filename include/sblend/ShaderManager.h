#ifndef __SHADERMANAGER_H__
#define __SHADERMANAGER_H__
#include "graphics/Shader.h"

namespace sx
{

    class ShaderManager
    {
        static Shader basicShader;

        static Shader lightningShader;

        static Shader pbrLightningShader;

        static Shader shadowMapShader;

        static ShaderManager* shaderManager;

        ShaderManager();

        public:
        static ShaderManager* getShaderManager();

        friend class Application;
    };
}

#endif // __SHADERMANAGER_H__