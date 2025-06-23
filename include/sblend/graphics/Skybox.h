
#ifndef SKYBOX_H
#define SKYBOX_H
#include "Mesh.h"

namespace sx {

class Skybox {
  Mesh cube;
  Texture texture;
public:
  Skybox(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices);

  void loadTexture(const std::vector<std::string>& faces);

  void render(Shader& shader);

  int getTextureID() const;

  ~Skybox();
};

} // sx

#endif //SKYBOX_H
