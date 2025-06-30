//
// Created by Silviu on 28.06.2025.
//

#include "MeshBoolean.h"

#include <cstddef>
#include <cstdint>
#include <iostream>
#include <map>
#include <vector>

#include "assimp/types.h"
#include "graphics/Vertex.h"
#include "manifold/manifold.h"
#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/matrix_decompose.hpp"

namespace sx {
void MeshBoolean::calculateNormals(std::vector<Vertex> &vertices,
                                   const std::vector<uint32_t> &indices) {
  for (auto &vertex : vertices)
    vertex.normal = glm::vec3(0.0f);

  for (size_t i = 0; i < indices.size(); i += 3) {
    uint32_t i0 = indices[i];
    uint32_t i1 = indices[i + 1];
    uint32_t i2 = indices[i + 2];

    glm::vec3 v0 = vertices[i0].position;
    glm::vec3 v1 = vertices[i1].position;
    glm::vec3 v2 = vertices[i2].position;

    glm::vec3 normal = glm::normalize(glm::cross(v1 - v0, v2 - v0));

    vertices[i0].normal += normal;
    vertices[i1].normal += normal;
    vertices[i2].normal += normal;
  }

  for (auto &vertex : vertices)
    vertex.normal = glm::normalize(vertex.normal);
}

void MeshBoolean::calculateTexCoords(std::vector<Vertex>& vertices)
{
    if(vertices.empty())
     return;

     glm::vec3 minPos = vertices[0].position;
     glm::vec3 maxPos = vertices[0].position;

     for(const auto& vertex : vertices)
     {
        minPos = glm::min(minPos, vertex.position);
        maxPos = glm::max(maxPos, vertex.position);
     }

     glm::vec3 extent = maxPos - minPos;
    if (extent.x == 0) extent.x = 1.0f;
    if (extent.y == 0) extent.y = 1.0f;

      for (auto& vertex : vertices) 
      {
        vertex.texCoords.x = (vertex.position.x - minPos.x) / extent.x;
        vertex.texCoords.y = (vertex.position.y - minPos.y) / extent.y;
    }
}

void MeshBoolean::calculateTangentsAndBitangents(
    std::vector<Vertex> &vertices, const std::vector<uint32_t> &indices) {
  std::vector<glm::vec3> tempTangents(vertices.size(), glm::vec3(0.0f));
  std::vector<glm::vec3> tempBitangents(vertices.size(), glm::vec3(0.0f));

  for (size_t i = 0; i < indices.size(); i += 3) {
    uint32_t i0 = indices[i];
    uint32_t i1 = indices[i + 1];
    uint32_t i2 = indices[i + 2];

    glm::vec3 pos0 = vertices[i0].position;
    glm::vec3 pos1 = vertices[i1].position;
    glm::vec3 pos2 = vertices[i2].position;

    glm::vec2 uv0 = vertices[i0].texCoords;
    glm::vec2 uv1 = vertices[i1].texCoords;
    glm::vec2 uv2 = vertices[i2].texCoords;

    glm::vec3 edge1 = pos1 - pos0;
    glm::vec3 edge2 = pos2 - pos0;
    glm::vec2 deltaUV1 = uv1 - uv0;
    glm::vec2 deltaUV2 = uv2 - uv0;

    float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

    glm::vec3 tangent;
    tangent.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
    tangent.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
    tangent.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);

    glm::vec3 bitangent;
    bitangent.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
    bitangent.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
    bitangent.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);

    tempTangents[i0] += tangent;
    tempTangents[i1] += tangent;
    tempTangents[i2] += tangent;

    tempBitangents[i0] += bitangent;
    tempBitangents[i1] += bitangent;
    tempBitangents[i2] += bitangent;
  }

  for (size_t i = 0; i < vertices.size(); ++i) {
    const glm::vec3 &n = vertices[i].normal;
    const glm::vec3 &t = tempTangents[i];
    const glm::vec3 &b = tempBitangents[i];

    glm::vec3 tangent = glm::normalize(t - n * glm::dot(n, t));

    glm::vec3 bitangent = glm::cross(n, tangent);
    if (glm::dot(glm::cross(n, tangent), b) < 0.0f)
      bitangent *= -1.0f;

    vertices[i].tangent = tangent;
    vertices[i].bitangent = bitangent;
  }
}

manifold::Manifold MeshBoolean::meshToManifold(const Mesh &mesh) {
  const auto &vertices = mesh.getVertices();
  const auto &indices = mesh.getIndices();

  std::map<Vertex, uint32_t> vertexToIndexMap;

  std::vector<Vertex> uniqueVertices;
  std::vector<uint32_t> newIndices;

  for (const auto &originalIndex : indices) {
    const auto &v = vertices[originalIndex];

    auto it = vertexToIndexMap.find(v);

    if (it == vertexToIndexMap.end()) {
      uniqueVertices.push_back(v);
      uint32_t newIndex = uniqueVertices.size() - 1;

      newIndices.push_back(newIndex);

      vertexToIndexMap[v] = newIndex;
    } else
      newIndices.push_back(it->second);
  }

  manifold::MeshGL meshGL;
  meshGL.vertProperties.resize(uniqueVertices.size() * 3);
  for (size_t i = 0; i < uniqueVertices.size(); ++i) {
    meshGL.vertProperties[i * 3] = uniqueVertices[i].position.x;
    meshGL.vertProperties[i * 3 + 1] = uniqueVertices[i].position.y;
    meshGL.vertProperties[i * 3 + 2] = uniqueVertices[i].position.z;
  }
  meshGL.numProp = 3;

  meshGL.triVerts = newIndices;

  return manifold::Manifold(meshGL);
}

void MeshBoolean::manifoldToMesh(const manifold::Manifold &manifold,
                                 std::vector<Vertex> &vertices,
                                 std::vector<uint32_t> &indices) {

  manifold::MeshGL meshGL = manifold.GetMeshGL();

  size_t numVerts = meshGL.vertProperties.size() / meshGL.numProp;
  size_t numTris = meshGL.triVerts.size();

  vertices.clear();
  vertices.reserve(numVerts);

  for (size_t i = 0; i < numVerts; ++i) {
    Vertex vertex;
    vertex.position = {meshGL.vertProperties[i * meshGL.numProp + 0],
                       meshGL.vertProperties[i * meshGL.numProp + 1],
                       meshGL.vertProperties[i * meshGL.numProp + 2]};

    vertex.texCoords = {0.0f, 0.0f};
    vertex.tangent = {1.0f, 0.0f, 0.0f};
    vertex.bitangent = {0.0f, 0.0f, 1.0f};

    vertices.push_back(vertex);
  }

  indices.clear();
  indices.reserve(numTris);
  for (size_t i = 0; i < numTris; ++i)
    indices.push_back(static_cast<uint32_t>(meshGL.triVerts[i]));

  calculateNormals(vertices, indices);
  calculateTexCoords(vertices);
  calculateTangentsAndBitangents(vertices, indices);
}

manifold::Manifold MeshBoolean::createManifoldFromMesh(const Mesh &mesh) {
  switch (mesh.type) {
  case MeshType::CUBE:
    return manifold::Manifold::Cube(manifold::vec3(1.f), true);
  case MeshType::SPHERE:
    return manifold::Manifold::Sphere(1.f, mesh.slices);
  case MeshType::CYLINDER:
    return manifold::Manifold::Cylinder(1.f, 1.f, 1.f, mesh.slices, true);
  case MeshType::CONE:
    return manifold::Manifold::Cylinder(1.f, 1.f, 0.f, mesh.slices, true);
  case MeshType::PLANE:
    return manifold::Manifold::Cube(manifold::vec3(1.0f, 1.0f, 0.01f), true);
  }
  return manifold::Manifold();
}

void MeshBoolean::applyMeshTransforms(manifold::Manifold &manifold,
                                      const Mesh &mesh) {
  manifold = manifold.Translate(
      manifold::vec3(mesh.position.x, mesh.position.y, mesh.position.z));
  manifold = manifold.Rotate(mesh.angles.x, mesh.angles.y, mesh.angles.z);
  manifold =
      manifold.Scale(manifold::vec3(mesh.scale.x, mesh.scale.y, mesh.scale.z));
}

void MeshBoolean::performOperation(Mesh &meshA, const Mesh &meshB,
                                   MeshBooleanOperation operation,
                                   std::vector<Vertex> &vertices,
                                   std::vector<uint32_t> &indices) {
  manifold::Manifold manifoldA = createManifoldFromMesh(meshA);
  manifold::Manifold manifoldB = createManifoldFromMesh(meshB);

  applyMeshTransforms(manifoldA, meshA);
  applyMeshTransforms(manifoldB, meshB);

  meshA.scale = {1.0f, 1.0f, 1.0f};
  meshA.angles = {0.0f, 0.0f, 0.0f};

  manifold::Manifold result;

  switch (operation) {
  case MeshBooleanOperation::UNION:
    result = manifoldA + manifoldB;
    break;

  case MeshBooleanOperation::SUBTRACT:
    result = manifoldA - manifoldB;
    break;

  case MeshBooleanOperation::INTERSECT:
    result = manifoldA ^ manifoldB;
    break;
  }

  manifoldToMesh(result, vertices, indices);
}
} // namespace sx
