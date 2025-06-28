#ifndef __MODEL_H__
#define __MODEL_H__
#include "../../assimp/Importer.hpp"
#include "../../assimp/postprocess.h"
#include "../../assimp/scene.h"
#include "Mesh.h"
#include <unordered_map>
#include <map>

struct RenderElementsIndirectCommand
{
  unsigned int count;
  unsigned int instanceCount;
  unsigned int firstIndex;
  unsigned int baseVertex;
  unsigned int baseInstance;
};

struct GpuMaterial
{
  glm::vec4 fallbackColor;
  int diffuseTexLayer;
  int specularTexLayer;
  int normalTexLayer;
  int heightTexLayer;
  int emissiveTexLayer;
  int ambientOcclusionTexLayer;
  int metalnessTexLayer;
  int roughnessTexLayer;
  int opacityTexLayer;
  int shininessTexLayer;
  int hasDiffuse;
  int hasSpecular;
  int hasNormal;
  int hasHeight;
  int hasEmissive;
  int hasAmbientOcclusion;
  int hasMetalness;
  int hasRoughness;
  int hasOpacity;
  int hasShininess;
  //char _padding[8];
};

static_assert(sizeof(GpuMaterial) % 16 == 0);

class Model
{
  std::string directory;

  std::vector<RenderElementsIndirectCommand> renderCommands;

  std::vector<Vertex> allVertices;
  std::vector<uint32_t> allIndices;

  void setupIndirectRendering();

  std::vector<GpuMaterial> gpuMaterials;

  std::vector<std::string> diffuseTexturePaths;
  std::vector<std::string> specularTexturePaths;
  std::vector<std::string> normalTexturePaths;
  std::vector<std::string> heightTexturePaths;
  std::vector<std::string> emissiveTexturePaths;
  std::vector<std::string> ambientOcclusionTexturePaths;
  std::vector<std::string> metalnessTexturePaths;
  std::vector<std::string> roughnessTexturePaths;
  std::vector<std::string> opacityTexturePaths;
  std::vector<std::string> shininessTexturePaths;

  std::map<std::string, int> diffusePathToIndex;
  std::map<std::string, int> specularPathToIndex;
  std::map<std::string, int> normalPathToIndex;
  std::map<std::string, int> heightPathToIndex;
  std::map<std::string, int> emissivePathToIndex;
  std::map<std::string, int> ambientOcclusionPathToIndex;
  std::map<std::string, int> metalnessPathToIndex;
  std::map<std::string, int> roughnessPathToIndex;
  std::map<std::string, int> opacityPathToIndex;
  std::map<std::string, int> shininessPathToIndex;

  void setupGpuResources();

  void setUpTextureArray(uint32_t& textureArray, const std::vector<std::string>& texturePaths);

  void setupPaths(aiMaterial* material, GpuMaterial& currentMaterial);

  void debugMaterials();

  void loadModel(const std::string &path);

  void processNode(aiNode *node, const aiScene *scene);

  void processMesh(aiMesh *mesh, const aiScene *scene);

  glm::mat4 model;

public:
  Model(const char *path);

  uint32_t materialUBO, diffuseTexArray, specularTexArray, normalTexArray, heightTexArray,
  emissiveTexArray, ambientOcclusionTexArray, metalnessTexArray, roughnessTexArray, opacityTexArray, shininessTexArray;

  uint32_t vbo, ebo, vao, indirectBuffer;

  glm::vec3 position;
  glm::vec3 velocity;
  glm::vec3 scale;
  glm::vec3 angles;
  glm::vec3 color;

  std::string path;

  const glm::mat4 &getModel();

  float metallic;
  float roughness;
  float ao;

  glm::vec3 boundingSphereCenter;
  float boundingSphereRadius;

  bool isReflective = false;

  int useFlatReflection = 1;

  void calculateBoundingSphere();

  void render(Shader &shader);

  void getProperties(std::ostringstream &stream);

  bool hasLoaded;
};
#endif // __MODEL_H__