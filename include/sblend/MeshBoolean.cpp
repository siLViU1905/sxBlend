//
// Created by Silviu on 28.06.2025.
//

#include "MeshBoolean.h"

#include <iostream>

#include "assimp/types.h"
#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/matrix_decompose.hpp"

namespace sx
{
    void MeshBoolean::calculateNormals(std::vector<Vertex> &vertices, const std::vector<uint32_t> &indices)
    {
        for (auto &vertex: vertices)
            vertex.normal = glm::vec3(0.0f);

        for (size_t i = 0; i < indices.size(); i += 3)
        {
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

        for (auto &vertex: vertices)
            vertex.normal = glm::normalize(vertex.normal);
    }

    void MeshBoolean::manifoldToMesh(const manifold::Manifold &manifold, std::vector<Vertex> &vertices,
                                     std::vector<uint32_t> &indices)
    {


        manifold::MeshGL meshGL = manifold.GetMeshGL();

        size_t numVerts = meshGL.vertProperties.size() / meshGL.numProp;
        size_t numTris = meshGL.triVerts.size();

        vertices.clear();
        vertices.reserve(numVerts);

        for (size_t i = 0; i < numVerts; ++i)
        {
            Vertex vertex;
            vertex.position = {
                meshGL.vertProperties[i * meshGL.numProp + 0],
                meshGL.vertProperties[i * meshGL.numProp + 1],
                meshGL.vertProperties[i * meshGL.numProp + 2]
            };

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
    }

    manifold::Manifold MeshBoolean::createManifoldFromMesh(const Mesh &mesh)
    {
        switch (mesh.type)
        {
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

    void MeshBoolean::applyMeshTransforms(manifold::Manifold &manifold, const Mesh &mesh)
    {
        manifold = manifold.Translate(manifold::vec3(mesh.position.x, mesh.position.y, mesh.position.z));
        manifold = manifold.Rotate(mesh.angles.x + 90.f, mesh.angles.y, mesh.angles.z);
        manifold = manifold.Scale(manifold::vec3(mesh.scale.x , mesh.scale.y , mesh.scale.z));
    }

    void MeshBoolean::performOperation(Mesh &meshA, const Mesh &meshB, MeshBooleanOperation operation,
                                       std::vector<Vertex> &vertices, std::vector<uint32_t> &indices)
    {
        manifold::Manifold manifoldA = createManifoldFromMesh(meshA);
        manifold::Manifold manifoldB = createManifoldFromMesh(meshB);


        applyMeshTransforms(manifoldA, meshA);
        applyMeshTransforms(manifoldB, meshB);

        manifold::Manifold result;

        switch (operation)
        {
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
        calculateNormals(vertices, indices);
    }
}
