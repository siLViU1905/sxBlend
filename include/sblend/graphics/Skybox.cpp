
#include "Skybox.h"

namespace sx
{
    Skybox::Skybox(const std::vector<Vertex> &vertices, const std::vector<uint32_t> &indices) : cube(vertices, indices)
    {
        path = "!";
    }

    void Skybox::loadTexture(const std::vector<std::string> &faces)
    {
        if (path != "!")
            glDeleteTextures(1, &texture.id);
        path = faces.front();
        texture.loadCubemap(faces);
    }

    void Skybox::render(Shader &shader)
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

    void Skybox::getProperties(std::ostringstream &stream)
    {
        stream.clear();
        stream.str("");

        stream << path << '\n';
    }

    Skybox::~Skybox()
    {
        glDeleteTextures(1, &texture.id);
    }
} // sx