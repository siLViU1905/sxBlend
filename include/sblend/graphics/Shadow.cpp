#include "Shadow.h"
namespace sx
{
    Shadow::Shadow(Shader &shader)
    {
        shadowMapShader = &shader;

        glGenFramebuffers(1, &depthFBO);

        glGenTextures(1, &depthMap);
        glBindTexture(GL_TEXTURE_2D, depthMap);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        float borderColor[] = {1.0f, 1.0f, 1.0f, 1.0f};

        glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

        glBindFramebuffer(GL_FRAMEBUFFER, depthFBO);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void Shadow::apply_to_mesh(Mesh &mesh)
    {
        shadowMapShader->setMat4("model", mesh.getModel());
        mesh.render(*shadowMapShader);
    }
    
    void Shadow::apply_to_model(Model& model)
    {
        shadowMapShader->setMat4("model", model.getModel());
        model.render(*shadowMapShader);
    }

    void Shadow::setLightProjection(const glm::mat4 &proj)
    {
        lightProjection = proj;
    }

    void Shadow::selLightView(const glm::mat4 &view)
    {
        lightView = view;
    }

    void Shadow::shadowMode(bool mode, int width, int height)
    {
        if (mode)
        {
            lightSpaceMatrix = lightProjection * lightView;
            shadowMapShader->setMat4("lightSpaceMatrix", lightSpaceMatrix);

            glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
            glBindFramebuffer(GL_FRAMEBUFFER, depthFBO);
            glClear(GL_DEPTH_BUFFER_BIT);
        }
        else
        {
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            glViewport(0, 0, width, height);
        }
    }

    void Shadow::apply_shadow_texture(Shader &meshShader)
    {
        glActiveTexture(GL_TEXTURE0 + SHADOW_TEXTURE_BIND);
        glBindTexture(GL_TEXTURE_2D, depthMap);

        meshShader.setMat4("lightSpaceMatrix", lightSpaceMatrix);
        meshShader.setInt("shadowMap", SHADOW_TEXTURE_BIND);
    }

    Shadow::~Shadow()
    {
        glDeleteTextures(1, &depthMap);
        glDeleteFramebuffers(1, &depthFBO);
    }
}