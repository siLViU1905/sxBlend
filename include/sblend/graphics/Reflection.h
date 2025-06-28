#ifndef REFLECTION_H
#define REFLECTION_H
#include "Mesh.h"
#include  "Model.h"
#include "../Camera.h"

namespace sx {

class Reflection {
  unsigned int fbo;
  unsigned int reflectionTexture;
  unsigned int rbo;
  Shader* reflectionShader;

  glm::mat4 view, projection;
  glm::mat4 calculateFlatReflectionViewMatrix(const Camera& camera, const Mesh& mesh);
    glm::mat4 calculateCurvedReflectionViewMatrix(const Camera& camera, const Mesh& mesh);

  glm::mat4 model_calculateFlatReflectionViewMatrix(const Camera& camera, const Model& model);
  glm::mat4 model_calculateCurvedReflectionViewMatrix(const Camera& camera, const Model& model);

public:
  Reflection(Shader& shader);

  void bind();

  void unbind();

  glm::mat4 calculateReflectionViewMatrix(const Camera& camera, const Mesh& mesh);

  glm::mat4 model_calculateReflectionViewMatrix(const Camera& camera, const Model& model);


  void renderSurface(Mesh& surfaceMesh, const Camera& mainCamera, const glm::mat4& mainProjection, int skyboxTex = -1);

  void renderSurface(Model& surfaceModel, const Camera& mainCamera, const glm::mat4& mainProjection, int skyboxTex = -1);

   void updateFrameBufferSize(int newWidth, int newHeight);

  ~Reflection();
};

inline Reflection::~Reflection()
{
  glDeleteTextures(1, &reflectionTexture);
  glDeleteFramebuffers(1, &fbo);
  glDeleteRenderbuffers(1, &rbo);
}
} // sx

#endif //REFLECTION_H
