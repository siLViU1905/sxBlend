#include "Model.h"
#include "../../stb_image.h"
#include <iostream>
#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include "../../stb_image_resize.h"

void Model::loadModel(const std::string &path)
{
  Assimp::Importer import;
  const aiScene *scene =
      import.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals |
                                aiProcess_FlipUVs | aiProcess_CalcTangentSpace);

  if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    throw std::runtime_error("");

  size_t separator_pos = path.find_last_of("/\\");

  if (std::string::npos != separator_pos)
    directory = path.substr(0, separator_pos + 1);
  else
    directory = "";

  processNode(scene->mRootNode, scene);
}

void Model::processNode(aiNode *node, const aiScene *scene)
{
  for (unsigned int i = 0; i < node->mNumMeshes; ++i)
  {
    aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
    processMesh(mesh, scene);
  }

  for (unsigned int i = 0; i < node->mNumChildren; ++i)
    processNode(node->mChildren[i], scene);
}

void Model::processMesh(aiMesh *mesh, const aiScene *scene)
{
  unsigned int baseVertex = allVertices.size();
  unsigned int firstIndex = allIndices.size();
  unsigned int indexCount = 0;

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

    allVertices.push_back(vertex);
  }

  for (unsigned int i = 0; i < mesh->mNumFaces; ++i)
  {
    aiFace face = mesh->mFaces[i];
    for (unsigned int j = 0; j < face.mNumIndices; ++j)
      allIndices.push_back(face.mIndices[j]);
  }

  indexCount = allIndices.size() - firstIndex;

  RenderElementsIndirectCommand cmd = {};
  cmd.count = indexCount;
  cmd.instanceCount = 1;
  cmd.firstIndex = firstIndex;
  cmd.baseVertex = baseVertex;
  cmd.baseInstance = 0;

  renderCommands.push_back(cmd);

  GpuMaterial currentMaterial = {};
  currentMaterial.fallbackColor = glm::vec4(color, 1.f);

  if (mesh->mMaterialIndex >= 0)
  {
    aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];

    setupPaths(material, currentMaterial);
  }
  gpuMaterials.push_back(currentMaterial);
}

Model::Model(const char *path) : path(path)
{
  model = glm::mat4(1.f);
  velocity = angles = position = glm::vec3(0.f);
  scale = glm::vec3(0.5f);
  color = glm::vec3(0.8f);
  metallic = roughness = ao = 0.5f;
  loadModel(path);
  setupIndirectRendering();

  setupGpuResources();


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
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
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

    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, pixels);
    glGenerateMipmap(GL_TEXTURE_2D);
  }
  else
    return false;

  stbi_image_free(pixels);

  return true;
}

void Model::setupIndirectRendering()
{
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);

  glGenBuffers(1, &vbo);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, allVertices.size() * sizeof(Vertex), allVertices.data(), GL_STATIC_DRAW);

  glGenBuffers(1, &ebo);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, allIndices.size() * sizeof(uint32_t), allIndices.data(), GL_STATIC_DRAW);

  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)0);

  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, normal));

  glEnableVertexAttribArray(2);
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, texCoords));

  glEnableVertexAttribArray(3);
  glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, tangent));

  glEnableVertexAttribArray(4);
  glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, bitangent));

  glGenBuffers(1, &indirectBuffer);
  glBindBuffer(GL_DRAW_INDIRECT_BUFFER, indirectBuffer);
  glBufferData(GL_DRAW_INDIRECT_BUFFER, renderCommands.size() * sizeof(RenderElementsIndirectCommand), renderCommands.data(), GL_STATIC_DRAW);

  glBindVertexArray(0);
}

void Model::setupGpuResources()
{
  setUpTextureArray(diffuseTexArray, diffuseTexturePaths);
  setUpTextureArray(specularTexArray, specularTexturePaths);
  setUpTextureArray(normalTexArray, normalTexturePaths);
  setUpTextureArray(heightTexArray, heightTexturePaths);
  setUpTextureArray(emissiveTexArray, emissiveTexturePaths);
  setUpTextureArray(ambientOcclusionTexArray, ambientOcclusionTexturePaths);
  setUpTextureArray(metalnessTexArray, metalnessTexturePaths);
  setUpTextureArray(roughnessTexArray, roughnessTexturePaths);
  setUpTextureArray(opacityTexArray, opacityTexturePaths);

  glGenBuffers(1, &materialUBO);
  glBindBuffer(GL_UNIFORM_BUFFER, materialUBO);
  glBufferData(GL_UNIFORM_BUFFER, gpuMaterials.size() * sizeof(GpuMaterial), gpuMaterials.data(), GL_STATIC_DRAW);
}

void Model::setUpTextureArray(uint32_t &textureArray, const std::vector<std::string> &texturePaths)
{
  int texWidth = 1024, texHeight = 1024;

  bool firstTexImage3D = false;

  glGenTextures(1, &textureArray);
  glBindTexture(GL_TEXTURE_2D_ARRAY, textureArray);
  int i =0;
  for (const auto& texPath : texturePaths)
  {
    std::string fullPath = directory + texPath;

    int width,height,channels;
    unsigned char* data = stbi_load(fullPath.c_str(), &width, &height, &channels, 0);
    if (data)
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

      if (!firstTexImage3D)
      {
        glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, format == GL_RGBA ? GL_RGBA8 : GL_RGB8, texWidth, texHeight, texturePaths.size(), 0, format, GL_UNSIGNED_BYTE, nullptr);
        firstTexImage3D = true;
      }
      if (width != texWidth || height != texHeight)
      {
        std::vector<unsigned char> resizedData(texWidth * texHeight * channels);
        stbir_resize_uint8(data, width, height, 0, resizedData.data(), texWidth, texHeight, 0, channels);
        glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, i,
                        texWidth, texHeight, 1, format, GL_UNSIGNED_BYTE,
                        resizedData.data());
      }
      else
        glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, i,
                        texWidth, texHeight, 1, format, GL_UNSIGNED_BYTE,
                        data);
       ++i;
      stbi_image_free(data);
    }
  }

  glGenerateMipmap(GL_TEXTURE_2D_ARRAY);
  glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

void Model::setupPaths(aiMaterial *material, GpuMaterial& currentMaterial)
{
  aiString str;
  if (material->GetTextureCount(aiTextureType_DIFFUSE) > 0)
  {
    material->GetTexture(aiTextureType_DIFFUSE, 0, &str);
    std::string texPath = str.C_Str();

    if (diffusePathToIndex.find(texPath) == diffusePathToIndex.end())
    {
      diffusePathToIndex[texPath] = diffuseTexturePaths.size();
      diffuseTexturePaths.push_back(texPath);
    }
    currentMaterial.diffuseTexLayer = diffusePathToIndex[texPath];
    currentMaterial.hasDiffuse = 1;
  }

  if (material->GetTextureCount(aiTextureType_SPECULAR) > 0)
  {
    material->GetTexture(aiTextureType_SPECULAR, 0, &str);
    std::string texPath = str.C_Str();
    if (specularPathToIndex.find(texPath) == specularPathToIndex.end())
    {
      specularPathToIndex[texPath] = specularTexturePaths.size();
      specularTexturePaths.push_back(texPath);
    }
    currentMaterial.specularTexLayer = specularPathToIndex[texPath];
    currentMaterial.hasSpecular = 1;
  }

  if (material->GetTextureCount(aiTextureType_EMISSIVE) > 0)
  {
    material->GetTexture(aiTextureType_EMISSIVE, 0, &str);
    std::string texPath = str.C_Str();
    if (emissivePathToIndex.find(texPath) == emissivePathToIndex.end())
    {
      emissivePathToIndex[texPath] = emissiveTexturePaths.size();
      emissiveTexturePaths.push_back(texPath);
    }
    currentMaterial.emissiveTexLayer = emissivePathToIndex[texPath];
    currentMaterial.hasEmissive = 1;
  }

  if (material->GetTextureCount(aiTextureType_HEIGHT) > 0)
  {
    material->GetTexture(aiTextureType_HEIGHT, 0, &str);
    std::string texPath = str.C_Str();
    if (heightPathToIndex.find(texPath) == heightPathToIndex.end())
    {
      heightPathToIndex[texPath] = heightTexturePaths.size();
      heightTexturePaths.push_back(texPath);
    }
    currentMaterial.heightTexLayer = heightPathToIndex[texPath];
    currentMaterial.hasHeight = 1;
  }

  if (material->GetTextureCount(aiTextureType_NORMALS) > 0)
  {
    material->GetTexture(aiTextureType_NORMALS, 0, &str);
    std::string texPath = str.C_Str();
    if (normalPathToIndex.find(texPath) == normalPathToIndex.end())
    {
      normalPathToIndex[texPath] = normalTexturePaths.size();
      normalTexturePaths.push_back(texPath);
    }
    currentMaterial.normalTexLayer = normalPathToIndex[texPath];
    currentMaterial.hasNormal = 1;
  }

  if (material->GetTextureCount(aiTextureType_OPACITY) > 0)
  {
    material->GetTexture(aiTextureType_OPACITY, 0, &str);
    std::string texPath = str.C_Str();
    if (opacityPathToIndex.find(texPath) == opacityPathToIndex.end())
    {
      opacityPathToIndex[texPath] = opacityTexturePaths.size();
      opacityTexturePaths.push_back(texPath);
    }
    currentMaterial.opacityTexLayer = opacityPathToIndex[texPath];
    currentMaterial.hasOpacity = 1;
  }

  if (material->GetTextureCount(aiTextureType_METALNESS) > 0)
  {
    material->GetTexture(aiTextureType_METALNESS, 0, &str);
    std::string texPath = str.C_Str();
    if (metalnessPathToIndex.find(texPath) == metalnessPathToIndex.end())
    {
      metalnessPathToIndex[texPath] = metalnessTexturePaths.size();
      metalnessTexturePaths.push_back(texPath);
    }
    currentMaterial.metalnessTexLayer = metalnessPathToIndex[texPath];
    currentMaterial.hasMetalness = 1;
  }

  if (material->GetTextureCount(aiTextureType_DIFFUSE_ROUGHNESS) > 0)
  {
    material->GetTexture(aiTextureType_DIFFUSE_ROUGHNESS, 0, &str);
    std::string texPath = str.C_Str();
    if (roughnessPathToIndex.find(texPath) == roughnessPathToIndex.end())
    {
      roughnessPathToIndex[texPath] = roughnessTexturePaths.size();
      roughnessTexturePaths.push_back(texPath);
    }
    currentMaterial.roughnessTexLayer = roughnessPathToIndex[texPath];
    currentMaterial.hasRoughness = 1;
  }

  if (material->GetTextureCount(aiTextureType_AMBIENT_OCCLUSION) > 0)
  {
    material->GetTexture(aiTextureType_AMBIENT_OCCLUSION, 0, &str);
    std::string texPath = str.C_Str();
    if (ambientOcclusionPathToIndex.find(texPath) == ambientOcclusionPathToIndex.end())
    {
      ambientOcclusionPathToIndex[texPath] = ambientOcclusionTexturePaths.size();
      ambientOcclusionTexturePaths.push_back(texPath);
    }
    currentMaterial.ambientOcclusionTexLayer = ambientOcclusionPathToIndex[texPath];
    currentMaterial.hasAmbientOcclusion = 1;
  }
}

void Model::debugMaterials()
{
  std::cout << "=== DEBUG MATERIALS ===" << std::endl;
  std::cout << "Total materials: " << gpuMaterials.size() << std::endl;
  std::cout << "Diffuse textures: " << diffuseTexturePaths.size() << std::endl;
  std::cout << "Specular textures: " << specularTexturePaths.size() << std::endl;

  for (int i = 0; i < gpuMaterials.size(); ++i)
  {
    const auto &mat = gpuMaterials[i];
    std::cout << "Material " << i << ":" << std::endl;
    //std::cout << "  hasTextures: " << mat.hasTextures << std::endl;
    std::cout << "  diffuseTexLayer: " << mat.diffuseTexLayer << std::endl;
    std::cout << "  specularTexLayer: " << mat.specularTexLayer << std::endl;
    std::cout << "  fallbackColor: " << mat.fallbackColor.r << ", "
              << mat.fallbackColor.g << ", " << mat.fallbackColor.b << std::endl;
  }

  std::cout << "Diffuse texture paths:" << std::endl;
  for (int i = 0; i < diffuseTexturePaths.size(); ++i)
  {
    std::cout << "  [" << i << "] " << diffuseTexturePaths[i] << std::endl;
  }

  std::cout << "Specular texture paths:" << std::endl;
  for (int i = 0; i < specularTexturePaths.size(); ++i)
  {
    std::cout << "  [" << i << "] " << specularTexturePaths[i] << std::endl;
  }
  std::cout << "======================" << std::endl;
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

  shader.setFloat("metallic", metallic);
  shader.setFloat("roughness", roughness);
  shader.setFloat("ao", ao);

  glBindBufferBase(GL_UNIFORM_BUFFER, 0, materialUBO);

  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D_ARRAY, diffuseTexArray);
  shader.setInt("diffuseTexArray", 1);

  glActiveTexture(GL_TEXTURE2);
  glBindTexture(GL_TEXTURE_2D_ARRAY, specularTexArray);
  shader.setInt("specularTexArray", 2);

  glActiveTexture(GL_TEXTURE3);
  glBindTexture(GL_TEXTURE_2D_ARRAY, normalTexArray);
  shader.setInt("normalTexArray", 3);

  glActiveTexture(GL_TEXTURE4);
  glBindTexture(GL_TEXTURE_2D_ARRAY, heightTexArray);
  shader.setInt("heightTexArray", 4);

  glBindVertexArray(vao);
  glBindBuffer(GL_DRAW_INDIRECT_BUFFER, indirectBuffer);

  glMultiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT,
                              (void *)0, renderCommands.size(), 0);

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
  glActiveTexture(GL_TEXTURE2);
  glBindTexture(GL_TEXTURE_2D, 0);
  glActiveTexture(GL_TEXTURE3);
  glBindTexture(GL_TEXTURE_2D, 0);
  glActiveTexture(GL_TEXTURE4);
  glBindTexture(GL_TEXTURE_2D, 0);

  glBindVertexArray(0);
}
