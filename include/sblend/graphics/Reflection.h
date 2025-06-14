#ifndef REFLECTION_H
#define REFLECTION_H
#include "Mesh.h"
#include "../Camera.h"

namespace sx {

class Reflection {
  unsigned int fbo;
  unsigned int reflectionTexture;
  unsigned int rbo;
  Shader* reflectionShader;

  glm::mat4 view, projection;
  public:
  Reflection(Shader& shader);

  void bind();

  void unbind();

  glm::mat4 calculateReflectionViewMatrix(const Camera& camera, float surfaceHeight);

  void renderSurface(Mesh& surfaceMesh, const Camera& mainCamera, const glm::mat4& mainProjection);

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
