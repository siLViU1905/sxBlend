
#include "Skybox.h"

namespace sx {
    Skybox::Skybox(const std::vector<Vertex> &vertices, const std::vector<uint32_t> &indices): cube(vertices, indices)
    {
    }

    void Skybox::loadTexture(const std::vector<std::string> &faces)
    {
        texture.loadCubemap(faces);
    }

    void Skybox::render(Shader& shader)
    {
        glDepthMask(GL_FALSE);
        glDepthFunc(GL_LEQUAL);
        glBindTexture(GL_TEXTURE_CUBE_MAP, texture.id);
        cube.renderForSkyboxUse(shader);
        glDepthMask(GL_TRUE);
        glDepthFunc(GL_LESS);
    }
    
    int Skybox::getTextureID() const
    {
        return texture.id;
    }

    Skybox::~Skybox()
    {
        glDeleteTextures(1, &texture.id);
    }
} // sx