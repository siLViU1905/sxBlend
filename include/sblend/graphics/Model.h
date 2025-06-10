#ifndef __MODEL_H__
#define __MODEL_H__
#include "../../assimp/Importer.hpp"
#include "../../assimp/postprocess.h"
#include "../../assimp/scene.h"
#include "Mesh.h"
#include <cstdint>
#include <unordered_map>


class Model {
  std::string directory;

  void loadModel(const std::string &path);

  void proccesNode(aiNode *node, const aiScene *scene);

  void proccesMesh(aiMesh *mesh, const aiScene *scene);

  void loadMaterialTextures(aiMaterial* mat, aiTextureType type);

  glm::mat4 model;

  static std::unordered_map<int, std::string> texType;

  static std::unordered_map<std::string, int> loadedTextures;

  static std::unordered_map<std::string, int> texCount;

  int hasTex = 0;

  static constexpr aiTextureType types[] = {
            aiTextureType_DIFFUSE,
            aiTextureType_SPECULAR,
            aiTextureType_AMBIENT,
            aiTextureType_EMISSIVE,
            aiTextureType_HEIGHT,
            aiTextureType_NORMALS,
            aiTextureType_SHININESS,
            aiTextureType_OPACITY,
            aiTextureType_DISPLACEMENT,
            aiTextureType_LIGHTMAP,
            aiTextureType_REFLECTION,
            aiTextureType_BASE_COLOR,
            aiTextureType_NORMAL_CAMERA,
            aiTextureType_EMISSION_COLOR,
            aiTextureType_METALNESS,
            aiTextureType_DIFFUSE_ROUGHNESS,
            aiTextureType_AMBIENT_OCCLUSION};

public:
  Model(const char *path);

  std::vector<Mesh> meshes;
  std::vector<Texture> textures;

  glm::vec3 position;
  glm::vec3 velocity;
  glm::vec3 scale;
  glm::vec3 angles;
  glm::vec3 color;

  const glm::mat4& getModel();

  float metallic;
  float roughness;
  float ao;

  void render(Shader &shader);
};
#endif // __MODEL_H__