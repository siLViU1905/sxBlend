#ifndef __TERRAIN_H__
#define __TERRAIN_H__
#include "graphics/Shader.h"
#include "graphics/Mesh.h"
#include <cstdint>
#include <vector>

namespace sx {
class Terrain 
{
  std::vector<Vertex> vertices;
  std::vector<uint32_t> indices;
 
  int width, height;
 

  float getHeightFromData(unsigned char *data, int x, int z);

  glm::vec3 calculateNormal(unsigned char *data, int x, int z);

  void calculateTangentBitagent(std::vector<Vertex> &vertices,
                                const std::vector<uint32_t> &indices);

  void setupMesh();

public:
  Terrain();

   Mesh* mesh = nullptr;

  bool loadHeightMap(const char *path);

  void render(Shader& shader);

   float maxHeight, scale;

  ~Terrain();
};
} // namespace sx

#endif // __TERRAIN_H__