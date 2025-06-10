#include "Model.h"
#include "../../stb_image.h"
#include <iostream>

std::unordered_map<int, std::string> Model::texType = {
    {aiTextureType_DIFFUSE, "diffuseTex"},
    {aiTextureType_SPECULAR, "specularTex"},
    {aiTextureType_NORMALS, "normalTex"},
    {aiTextureType_DIFFUSE_ROUGHNESS, "roughnessTex"},
    {aiTextureType_AMBIENT_OCCLUSION, "aoTex"},
    {aiTextureType_EMISSIVE, "emissiveTex"},
    {aiTextureType_HEIGHT, "heightTex"},
    {aiTextureType_METALNESS, "metalicTex"}
};

std::unordered_map<std::string, int> Model::loadedTextures;

std::unordered_map<std::string, int> Model::texCount;

void Model::loadModel(const std::string &path)
{
  Assimp::Importer import;
  const aiScene *scene =
      import.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals |
                                aiProcess_FlipUVs | aiProcess_CalcTangentSpace);

  if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    throw std::runtime_error("");

  directory = path.substr(0, path.find_last_of('/'));
  directory.push_back('/');

  proccesNode(scene->mRootNode, scene);
}

void Model::proccesNode(aiNode *node, const aiScene *scene)
{
  for (unsigned int i = 0; i < node->mNumMeshes; ++i)
  {
    aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
    proccesMesh(mesh, scene);
  }

  for (unsigned int i = 0; i < node->mNumChildren; ++i)
    proccesNode(node->mChildren[i], scene);
}

void Model::proccesMesh(aiMesh *mesh, const aiScene *scene)
{
  std::vector<Vertex> vertices;
  std::vector<unsigned int> indices;

  for (unsigned int i = 0; i < mesh->mNumVertices; ++i)
  {
    Vertex vertex;
    glm::vec3 vector;
    vector.x = mesh->mVertices[i].x;
    vector.y = mesh->mVertices[i].y;
    vector.z = mesh->mVertices[i].z;
    vertex.position = vector;

    if (mesh->HasNormals())
    {
      vector.x = mesh->mNormals[i].x;
      vector.y = mesh->mNormals[i].y;
      vector.z = mesh->mNormals[i].z;
      vertex.normal = vector;
    }

    if (mesh->mTextureCoords[0])
    {
      glm::vec2 vec;
      vec.x = mesh->mTextureCoords[0][i].x;
      vec.y = mesh->mTextureCoords[0][i].y;
      vertex.texCoords = vec;

      if (mesh->HasTangentsAndBitangents())
      {
        vector.x = mesh->mTangents[i].x;
        vector.y = mesh->mTangents[i].y;
        vector.z = mesh->mTangents[i].z;
        vertex.tangent = vector;

        vector.x = mesh->mBitangents[i].x;
        vector.y = mesh->mBitangents[i].y;
        vector.z = mesh->mBitangents[i].z;
        vertex.bitangent = vector;
      }
    }
    else
      vertex.texCoords = glm::vec2(0.f);

    vertices.push_back(vertex);
  }

  for (unsigned int i = 0; i < mesh->mNumFaces; ++i)
  {
    aiFace face = mesh->mFaces[i];
    for (unsigned int j = 0; j < face.mNumIndices; ++j)
      indices.push_back(face.mIndices[j]);
  }

  if (mesh->mMaterialIndex >= 0)
  {
    aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];

    for (const auto &type : types)
      loadMaterialTextures(material, type);
  }

  meshes.emplace_back(vertices, indices, textures);

  std::vector<Texture>().swap(textures);
}

void Model::loadMaterialTextures(aiMaterial *mat, aiTextureType type)
{
  for (int i = 0; i < mat->GetTextureCount(type); ++i)
  {
    hasTex = 1;
    aiString str;
    mat->GetTexture(type, i, &str);
    std::string texPath = str.C_Str();

    if (!loadedTextures[str.C_Str()])
    {
      loadedTextures[str.C_Str()] = 1;
      Texture texture;

      std::string fullPath = directory + texPath;
      std::string filename = texPath;
      size_t lastSlash = texPath.find_last_of("/\\");

      if (lastSlash != std::string::npos)
        filename = texPath.substr(lastSlash + 1);

      texture.load(fullPath.c_str(), texType[type]);
      ++texCount[texType[type]];
      textures.push_back(texture);
    }
  }
}

Model::Model(const char *path) : path(path)
{
  model = glm::mat4(1.f);
  velocity = angles = position = glm::vec3(0.f);
  scale = glm::vec3(0.5f);
  color = glm::vec3(0.8f);
  metallic = roughness = ao = 0.5f;
  loadModel(path);
}

const glm::mat4 &Model::getModel()
{
  model = glm::mat4(1.f);
  model = glm::translate(model, position);
  model = glm::rotate(model, glm::radians(angles.x), glm::vec3(1.f, 0.f, 0.f));
  model = glm::rotate(model, glm::radians(angles.y), glm::vec3(0.f, 1.f, 0.f));
  model = glm::rotate(model, glm::radians(angles.z), glm::vec3(0.f, 0.f, 1.f));
  model = glm::scale(model, scale);

  return model;
}

void Model::render(Shader &shader)
{
  model = glm::mat4(1.f);
  model = glm::translate(model, position);
  model = glm::rotate(model, glm::radians(angles.x), glm::vec3(1.f, 0.f, 0.f));
  model = glm::rotate(model, glm::radians(angles.y), glm::vec3(0.f, 1.f, 0.f));
  model = glm::rotate(model, glm::radians(angles.z), glm::vec3(0.f, 0.f, 1.f));
  model = glm::scale(model, scale);

  shader.setMat4("model", model);
  shader.setVec3("aColor", color);
  shader.setInt("hasTex", hasTex);

  shader.setFloat("metallic", metallic);
  shader.setFloat("roughness", roughness);
  shader.setFloat("ao", ao);

  for (Mesh &mesh : meshes)
    mesh.renderForModelUse(shader);
}

void Model::getProperties(std::ostringstream &stream)
{
  stream.clear();
  stream.str("");

  stream << path << '\n';

  stream << position.x << ' ' << position.y << ' ' << position.z << '\n';
  stream << velocity.x << ' ' << velocity.y << ' ' << velocity.z << '\n';
  stream << scale.x << ' ' << scale.y << ' ' << scale.z << '\n';
  stream << angles.x << ' ' << angles.y << ' ' << angles.z << '\n';
  stream << color.x << ' ' << color.y << ' ' << color.z << '\n';
  stream << metallic << '\n';
  stream << roughness << '\n';
  stream << ao << '\n';
}

bool Texture::load(const char *filepath, const std::string &type)
{
  this->type = type;

  glGenTextures(1, &id);
  glBindTexture(GL_TEXTURE_2D, id);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  stbi_set_flip_vertically_on_load(1);

  int width, height, channels;
  unsigned char *pixels = stbi_load(filepath, &width, &height, &channels, 0);

  if (pixels)
  {
    GLenum format;
    switch (channels)
    {
    case 1:
      format = GL_RED;
      break;

    case 2:
      format = GL_RG;
      break;

    case 3:
      format = GL_RGB;
      break;

    case 4:
      format = GL_RGBA;
      break;
    }

    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format,GL_UNSIGNED_BYTE, pixels);
    glGenerateMipmap(GL_TEXTURE_2D);
  }
  else
    return false;

  stbi_image_free(pixels);

  return true;
}
