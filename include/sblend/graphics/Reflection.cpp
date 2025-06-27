#include "Reflection.h"
#include <stdexcept>

namespace sx
{
    Reflection::Reflection(Shader &shader)
    {
        reflectionShader = &shader;

        glGenFramebuffers(1, &fbo);
        glBindFramebuffer(GL_FRAMEBUFFER, fbo);

        int scrWidth, scrHeight;

        glfwGetFramebufferSize(glfwGetCurrentContext(), &scrWidth, &scrHeight);
        if (scrWidth == 0 || scrHeight == 0)
        {
            scrWidth = 600;
            scrHeight = 600;
        }

        glGenTextures(1, &reflectionTexture);
        glBindTexture(GL_TEXTURE_2D, reflectionTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, scrWidth, scrHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, reflectionTexture, 0);

        glGenRenderbuffers(1, &rbo);
        glBindRenderbuffer(GL_RENDERBUFFER, rbo);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, scrWidth, scrHeight);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void Reflection::bind()
    {
        glBindFramebuffer(GL_FRAMEBUFFER, fbo);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    void Reflection::unbind()
    {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    glm::mat4 Reflection::calculateReflectionViewMatrix(const Camera &camera, const Mesh& mesh)
    {
        glm::mat4 reflectionView;

        if (mesh.type == MeshType::PLANE || mesh.type == MeshType::CUBE)
        {
            reflectionView = calculateFlatReflectionViewMatrix(camera, mesh);
            useFlat = 1;
        }
        else
        {
            reflectionView = calculateCurvedReflectionViewMatrix(camera, mesh);
           useFlat = 0;
        }

        return reflectionView;
    }

    glm::mat4 Reflection::calculateFlatReflectionViewMatrix(const Camera &camera, const Mesh &mesh)
    {
        float distance = 2.f * (camera.getPosition().y - mesh.position.y);
        glm::vec3 reflectionCamPos = camera.getPosition();
        reflectionCamPos.y = 2 * mesh.position.y - reflectionCamPos.y;


        return glm::lookAt(reflectionCamPos,
                           reflectionCamPos + camera.getFront(),
                           camera.getUp());
    }

    glm::mat4 Reflection::calculateCurvedReflectionViewMatrix(const Camera &camera, const Mesh &mesh)
    {
        auto camToMesh = camera.getPosition() - mesh.position;
        float distance = glm::length(camToMesh);
        float radius = (mesh.scale.x + mesh.scale.y + mesh.scale.z) / 3.f;
        auto reflectionCamPos = mesh.position + camToMesh * (radius * radius / (distance * distance));
        return glm::lookAt(reflectionCamPos,
                           reflectionCamPos + camera.getFront(),
                           camera.getUp());
    }

    void Reflection::renderSurface(Mesh &surfaceMesh, const Camera &mainCamera, const glm::mat4 &mainProjection,
                                   int skyboxTex)
    {

        auto reflectionView = calculateReflectionViewMatrix(mainCamera, surfaceMesh);

        reflectionShader->use();

        reflectionShader->setMat4("camera.view", mainCamera.getView());
        reflectionShader->setMat4("projection", mainProjection);
        reflectionShader->setMat4("reflectionViewMatrix", reflectionView);
        reflectionShader->setInt("useFlatReflection", useFlat);

        reflectionShader->setInt("reflectionTexture", 0);
        if (skyboxTex != -1)
        {
            reflectionShader->setInt("useSkybox", 1);
            reflectionShader->setInt("skybox", 1);
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTex);
        } else
            reflectionShader->setInt("useSkybox", 0);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, reflectionTexture);

        surfaceMesh.render(*reflectionShader);
    }

    void Reflection::updateFrameBufferSize(int newWidth, int newHeight)
    {
        glBindTexture(GL_TEXTURE_2D, reflectionTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, newWidth, newHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
        glBindTexture(GL_TEXTURE_2D, 0);

        glBindRenderbuffer(GL_RENDERBUFFER, rbo);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, newWidth, newHeight);
        glBindRenderbuffer(GL_RENDERBUFFER, 0);
    }
} // namespace sx
