#ifndef __TERRAIN_H__
#define __TERRAIN_H__
#include "graphics/Shader.h"
#include "graphics/Mesh.h"
#include <cstdint>
#include <type_traits>
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

  uint32_t textures[3];

public:
  Terrain();

   Mesh* mesh = nullptr;

  bool loadHeightMap(const char *path);

  void render(Shader& shader);

   float maxHeight, scale;

   glm::vec3 grassColor;
   glm::vec3 rockColor;
   glm::vec3 snowColor;

   bool hasTextures = false;

   bool loadTextures(const std::vector<std::string>& paths);

  ~Terrain();
};
} // namespace sx

#endif // __TERRAIN_H__