#ifndef __SHADOW_H__
#define __SHADOW_H__
#include "Mesh.h"

namespace sx
{
    class Shadow
    {
        unsigned int depthFBO, depthMap;

        Shader *shadowMapShader;

        static constexpr unsigned int SHADOW_WIDTH = 2048, SHADOW_HEIGHT = 2048;

        static constexpr unsigned int SHADOW_TEXTURE_BIND = 10;

    public:
        Shadow(Shader &shader);

        glm::mat4 lightView, lightProjection, lightSpaceMatrix;

        void apply_to_mesh(Mesh &mesh);

        void setLightProjection(const glm::mat4 &proj);

        void selLightView(const glm::mat4 &view);

        void shadowMode(bool mode, int width, int height);

        void apply_shadow_texture(Shader &meshShader);

        ~Shadow();
    };
}

#endif // __SHADOW_H__