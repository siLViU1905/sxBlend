#include "Terrain.h"
#include "Application.h"
#include "graphics/Shader.h"
#include "graphics/Vertex.h"
#include "stb_image.h"
#include <cstdint>

namespace sx {
float Terrain::getHeightFromData(unsigned char *data, int x, int z) {
  if (x < 0 || x >= width || z < 0 || z >= height)
    return 0.0f;
  return (data[z * width + x] / 255.0f) * maxHeight;
}

glm::vec3 Terrain::calculateNormal(unsigned char *data, int x, int z) {
  float heightL = getHeightFromData(data, x - 1, z);
  float heightR = getHeightFromData(data, x + 1, z);
  float heightD = getHeightFromData(data, x, z - 1);
  float heightU = getHeightFromData(data, x, z + 1);

  glm::vec3 normal =
      glm::normalize(glm::vec3((heightL - heightR) / (2.0f * scale), 1.0f,
                               (heightD - heightU) / (2.0f * scale)));

  return normal;
}

void Terrain::calculateTangentBitagent(std::vector<Vertex> &vertices,
                                       const std::vector<uint32_t> &indices) {
  for (auto &vertex : vertices)
  {
    vertex.tangent = glm::vec3(0.0f);
    vertex.bitangent = glm::vec3(0.0f);
  }

  for (int i = 0; i < indices.size(); i += 3)
  {
    Vertex &v0 = vertices[indices[i]];
    Vertex &v1 = vertices[indices[i + 1]];
    Vertex &v2 = vertices[indices[i + 2]];

    glm::vec3 deltaPos1 = v1.position - v0.position;
    glm::vec3 deltaPos2 = v2.position - v0.position;

    glm::vec2 deltaUV1 = v1.texCoords - v0.texCoords;
    glm::vec2 deltaUV2 = v2.texCoords - v0.texCoords;

    float r = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x);
    glm::vec3 tangent = (deltaPos1 * deltaUV2.y - deltaPos2 * deltaUV1.y) * r;
    glm::vec3 bitangent = (deltaPos2 * deltaUV1.x - deltaPos1 * deltaUV2.x) * r;

    v0.tangent += tangent;
    v1.tangent += tangent;
    v2.tangent += tangent;

    v0.bitangent += bitangent;
    v1.bitangent += bitangent;
    v2.bitangent += bitangent;
  }

  for (auto &vertex : vertices)
  {
    vertex.tangent = glm::normalize(vertex.tangent);
    vertex.bitangent = glm::normalize(vertex.bitangent);
  }
}

void Terrain::setupMesh() { mesh = new Mesh(vertices, indices); }

Terrain::Terrain() {
  grassColor = glm::vec3(0.2f, 0.6f, 0.2f);
  rockColor = glm::vec3(0.4f);
  snowColor = glm::vec3(0.95f);
}

bool Terrain::loadHeightMap(const char *path) {
  int channels;
  heightMapPath = path;
  unsigned char *data = stbi_load(path, &width, &height, &channels, 1);

  if (mesh)
    delete mesh;

  if (!data)
    return false;
  vertices.clear();
  indices.clear();

  for (int z = 0; z < height; z++)
  {
    for (int x = 0; x < width; x++)
    {
      Vertex vertex;

      vertex.position.x = x * scale;
      vertex.position.y = getHeightFromData(data, x, z);
      vertex.position.z = z * scale;

      vertex.normal = calculateNormal(data, x, z);

      vertex.texCoords.x = (float)x / (width - 1);
      vertex.texCoords.y = (float)z / (height - 1);

      vertices.push_back(vertex);
    }
  }

  for (int z = 0; z < height - 1; z++)
  {
    for (int x = 0; x < width - 1; x++)
    {
      int topLeft = z * width + x;
      int topRight = topLeft + 1;
      int bottomLeft = (z + 1) * width + x;
      int bottomRight = bottomLeft + 1;

      indices.push_back(topLeft);
      indices.push_back(bottomLeft);
      indices.push_back(topRight);

      indices.push_back(topRight);
      indices.push_back(bottomLeft);
      indices.push_back(bottomRight);
    }
  }

  calculateTangentBitagent(vertices, indices);

  setupMesh();

  stbi_image_free(data);

  return true;
}

void Terrain::render(Shader &shader) {
  if (mesh)
  {
    shader.setFloat("maxHeight", maxHeight);
    shader.setVec3("grassColor", grassColor);
    shader.setVec3("rockColor", rockColor);
    shader.setVec3("snowColor", snowColor);

    shader.setInt("hasTex", hasTextures);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textures[0]);
    shader.setInt("grassTexture", 0);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, textures[1]);
    shader.setInt("rockTexture", 1);

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, textures[2]);
    shader.setInt("snowTexture", 2);

    mesh->renderForTerrainUse(shader);

    for (int i = 0; i < 3; ++i)
    {
      glActiveTexture(GL_TEXTURE0 + i);
      glBindTexture(GL_TEXTURE_2D, 0);
    }
  }
}

bool Terrain::loadTextures(const std::vector<std::string> &paths) {
  glGenTextures(3, textures);

  for (int i = 0; i < paths.size(); ++i)
  {
    glBindTexture(GL_TEXTURE_2D, textures[i]);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                    GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int width, height, channels;
    unsigned char *data =
        stbi_load(paths[i].c_str(), &width, &height, &channels, 0);

    if (!data)
      return false;

    GLenum format;
    switch (channels)
    {
    case 1:
      format = GL_RED;
      break;

    case 3:
      format = GL_RGB;
      break;

    case 4:
      format = GL_RGBA;
      break;
    }

    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format,
                 GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(data);
  }

  this->paths = paths;
  hasTextures = true;
  return true;
}

void Terrain::getProperties(std::ostringstream &stream) const {
  stream.clear();
  stream.str("");

  stream << heightMapPath << '\n';

    stream << mesh->position.x << ' ' << mesh->position.y << ' ' << mesh->position.z << '\n';
    stream << mesh->velocity.x << ' ' << mesh->velocity.y << ' ' << mesh->velocity.z << '\n';
    stream << mesh->scale.x << ' ' <<    mesh->scale.y << ' ' <<    mesh->scale.z << '\n';
    stream << mesh->angles.x << ' ' <<   mesh->angles.y << ' ' <<   mesh->angles.z << '\n';
    stream << mesh->color.x << ' ' <<    mesh->color.y << ' ' <<    mesh->color.z << '\n';
    stream << mesh->metallic << '\n';
    stream << mesh->roughness << '\n';
    stream << mesh->ao << '\n';

    stream << maxHeight << '\n';
    stream << scale << '\n';

  if (!hasTextures)
    stream << "!";
  else
    for (const auto &path : paths)
      stream << path << '\n';
}

Terrain::~Terrain() {
  if (mesh)
  {
    glDeleteBuffers(1, &mesh->vbo);
    glDeleteBuffers(1, &mesh->ebo);
    glDeleteVertexArrays(1, &mesh->vao);
    delete mesh;
  }

  if (hasTextures)
    glDeleteTextures(3, textures);
}

} // namespace sx