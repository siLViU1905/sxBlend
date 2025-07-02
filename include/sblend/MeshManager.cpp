#include "MeshManager.h"

namespace sx
{
    MeshManager::MeshManager()
    {
    }

    void MeshManager::calculateTangentBitangent(const Vertex &v0, const Vertex &v1, const Vertex &v2,
                                                glm::vec3 &tangent, glm::vec3 &bitangent)
    {
        glm::vec3 edge1 = v1.position - v0.position;
        glm::vec3 edge2 = v2.position - v0.position;

        glm::vec2 deltaUV1 = v1.texCoords - v0.texCoords;
        glm::vec2 deltaUV2 = v2.texCoords - v0.texCoords;

        float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

        tangent.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
        tangent.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
        tangent.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);

        bitangent.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
        bitangent.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
        bitangent.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);

        tangent = glm::normalize(tangent);
        bitangent = glm::normalize(bitangent);
    }

    void MeshManager::calculateTangentSpace(std::vector<Vertex> &vertices, const std::vector<uint32_t> &indices)
    {
        for (auto &vertex : vertices)
        {
            vertex.tangent = glm::vec3(0.0f);
            vertex.bitangent = glm::vec3(0.0f);
        }

        for (size_t i = 0; i < indices.size(); i += 3)
        {
            uint32_t i0 = indices[i];
            uint32_t i1 = indices[i + 1];
            uint32_t i2 = indices[i + 2];

            glm::vec3 tangent, bitangent;
            calculateTangentBitangent(vertices[i0], vertices[i1], vertices[i2], tangent, bitangent);

            vertices[i0].tangent += tangent;
            vertices[i0].bitangent += bitangent;
            vertices[i1].tangent += tangent;
            vertices[i1].bitangent += bitangent;
            vertices[i2].tangent += tangent;
            vertices[i2].bitangent += bitangent;
        }

        for (auto &vertex : vertices)
        {

            vertex.tangent = glm::normalize(vertex.tangent - vertex.normal * glm::dot(vertex.normal, vertex.tangent));

            vertex.bitangent = glm::cross(vertex.normal, vertex.tangent);
            vertex.bitangent = glm::normalize(vertex.bitangent);
        }
    }

    void MeshManager::generateGridLines(std::vector<Vertex> &vertices, std::vector<uint32_t> &indices)
    {
        int j = 0;
        float length = 120.f;
        float step = 2.2f;
        for (float i = -length; i < length; i += step)
        {
            Vertex vertex;
            vertex.position = glm::vec3(i, 0.f, length);
            vertices.push_back(vertex);
            vertex.position = glm::vec3(i, 0.f, -length);
            vertices.push_back(vertex);

            indices.push_back(j);
            indices.push_back(j + 1);
            j += 2;
        }

        for (float i = -length; i < length; i += step)
        {
            Vertex vertex;
            vertex.position = glm::vec3(-length, 0.f, i);
            vertices.push_back(vertex);
            vertex.position = glm::vec3(length, 0.f, i);
            vertices.push_back(vertex);

            indices.push_back(j);
            indices.push_back(j + 1);
            j += 2;
        }
    }

    void MeshManager::generatePlaneMesh(std::vector<Vertex> &vertices, std::vector<uint32_t> &indices)
    {
        vertices = {

            {{-1.0f, 0.0f, -1.0f},
             {0.0f, 1.0f, 0.0f},
             {0.0f, 0.0f},
             {1.0f, 0.0f, 0.0f},
             {0.0f, 0.0f, 1.0f}},

            {{1.0f, 0.0f, -1.0f},
             {0.0f, 1.0f, 0.0f},
             {1.0f, 0.0f},
             {1.0f, 0.0f, 0.0f},
             {0.0f, 0.0f, 1.0f}},

            {{1.0f, 0.0f, 1.0f},
             {0.0f, 1.0f, 0.0f},
             {1.0f, 1.0f},
             {1.0f, 0.0f, 0.0f},
             {0.0f, 0.0f, 1.0f}},

            {{-1.0f, 0.0f, 1.0f},
             {0.0f, 1.0f, 0.0f},
             {0.0f, 1.0f},
             {1.0f, 0.0f, 0.0f},
             {0.0f, 0.0f, 1.0f}}};

        indices = {
            0, 1, 2,
            0, 2, 3};
    }

    void MeshManager::generateCircleMesh(std::vector<Vertex> &vertices, std::vector<uint32_t> &indices, int segments)
    {
        float radius = 1.0f;

        vertices.push_back({{0.0f, 0.0f, 0.0f},
                            {0.0f, 0.0f, 1.0f},
                            {0.5f, 0.5f},
                            {1.0f, 0.0f, 0.0f},
                            {0.0f, 1.0f, 0.0f}});

        for (int i = 0; i < segments; ++i)
        {
            float angle = 2.0f * 3.14f * i / segments;
            float x = radius * cos(angle);
            float y = radius * sin(angle);

            float u = (x + 1.0f) * 0.5f;
            float v = (y + 1.0f) * 0.5f;

            glm::vec3 tangent = glm::vec3(-std::sin(angle), std::cos(angle), 0.0f);
            glm::vec3 bitangent = glm::vec3(std::cos(angle), std::sin(angle), 0.0f);

            vertices.push_back({{x, y, 0.0f},
                                {0.0f, 0.0f, 1.0f},
                                {u, v},
                                tangent,
                                bitangent});
        }
        // Generate indices for triangles
        for (int i = 0; i < segments; ++i)
        {
            indices.push_back(0);
            indices.push_back(i + 1);
            indices.push_back((i + 1) % segments + 1);
        }
    }

    void MeshManager::generateCubeMesh(std::vector<Vertex> &vertices, std::vector<uint32_t> &indices)
    {
        float halfSize = 0.5f;

        vertices = {
            // Front face (Z+)
            {
                {-halfSize, -halfSize, halfSize}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}}, // 0
            {{halfSize, -halfSize, halfSize}, {0.0f, 0.0f, 1.0f}, {1.0f, 0.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}},
            // 1
            {{halfSize, halfSize, halfSize}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}},
            // 2
            {{-halfSize, halfSize, halfSize}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}},
            // 3

            // Back face (Z-)
            {
                {-halfSize, -halfSize, -halfSize}, {0.0f, 0.0f, -1.0f}, {1.0f, 0.0f}, {-1.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}}, // 4
            {
                {halfSize, -halfSize, -halfSize}, {0.0f, 0.0f, -1.0f}, {0.0f, 0.0f}, {-1.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}}, // 5
            {
                {halfSize, halfSize, -halfSize}, {0.0f, 0.0f, -1.0f}, {0.0f, 1.0f}, {-1.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}}, // 6
            {
                {-halfSize, halfSize, -halfSize}, {0.0f, 0.0f, -1.0f}, {1.0f, 1.0f}, {-1.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}}, // 7

            // Left face (X-)
            {
                {-halfSize, -halfSize, -halfSize}, {-1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f, 0.0f}}, // 8
            {
                {-halfSize, -halfSize, halfSize}, {-1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f, 0.0f}}, // 9
            {
                {-halfSize, halfSize, halfSize}, {-1.0f, 0.0f, 0.0f}, {1.0f, 1.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f, 0.0f}}, // 10
            {
                {-halfSize, halfSize, -halfSize}, {-1.0f, 0.0f, 0.0f}, {0.0f, 1.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f, 0.0f}}, // 11

            // Right face (X+)
            {
                {halfSize, -halfSize, -halfSize}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}, {0.0f, 0.0f, -1.0f}, {0.0f, 1.0f, 0.0f}}, // 12
            {
                {halfSize, -halfSize, halfSize}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}, {0.0f, 0.0f, -1.0f}, {0.0f, 1.0f, 0.0f}}, // 13
            {{halfSize, halfSize, halfSize}, {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f}, {0.0f, 0.0f, -1.0f}, {0.0f, 1.0f, 0.0f}},
            // 14
            {
                {halfSize, halfSize, -halfSize}, {1.0f, 0.0f, 0.0f}, {1.0f, 1.0f}, {0.0f, 0.0f, -1.0f}, {0.0f, 1.0f, 0.0f}}, // 15

            // Top face (Y+)
            {
                {-halfSize, halfSize, -halfSize}, {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 1.0f}}, // 16
            {{halfSize, halfSize, -halfSize}, {0.0f, 1.0f, 0.0f}, {1.0f, 1.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 1.0f}},
            // 17
            {{halfSize, halfSize, halfSize}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 1.0f}},
            // 18
            {{-halfSize, halfSize, halfSize}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 1.0f}},
            // 19

            // Bottom face (Y-)
            {
                {-halfSize, -halfSize, -halfSize}, {0.0f, -1.0f, 0.0f}, {0.0f, 0.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f, -1.0f}}, // 20
            {
                {halfSize, -halfSize, -halfSize}, {0.0f, -1.0f, 0.0f}, {1.0f, 0.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f, -1.0f}}, // 21
            {
                {halfSize, -halfSize, halfSize}, {0.0f, -1.0f, 0.0f}, {1.0f, 1.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f, -1.0f}}, // 22
            {
                {-halfSize, -halfSize, halfSize}, {0.0f, -1.0f, 0.0f}, {0.0f, 1.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f, -1.0f}} // 23
        };

        indices = {

            0, 1, 2, 2, 3, 0,

            4, 7, 6, 6, 5, 4,

            8, 11, 10, 10, 9, 8,

            12, 13, 14, 14, 15, 12,

            16, 17, 18, 18, 19, 16,

            20, 23, 22, 22, 21, 20};
    }

    void MeshManager::generateConeMesh(std::vector<Vertex> &vertices, std::vector<uint32_t> &indices, int segments)
    {
        float radius = 1.0f, height = 2.0f;

        vertices.clear();
        indices.clear();

        const float PI = 3.141592653f;
        Vertex apex;
        apex.position = glm::vec3(0.f, height * 0.5f, 0.f);
        apex.normal = glm::vec3(0.0f, 1.0f, 0.0f);
        apex.texCoords = glm::vec2(0.5f, 1.0f);
        apex.tangent = glm::vec3(1.0f, 0.0f, 0.0f);
        apex.bitangent = glm::vec3(0.0f, 0.0f, 1.0f);
        vertices.push_back(apex);

        Vertex baseCenter;
        baseCenter.position = glm::vec3(0.0f, -height * 0.5f, 0.0f);
        baseCenter.normal = glm::vec3(0.0f, -1.0f, 0.0f);
        baseCenter.texCoords = glm::vec2(0.5f, 0.0f);
        baseCenter.tangent = glm::vec3(1.0f, 0.0f, 0.0f);
        baseCenter.bitangent = glm::vec3(0.0f, 0.0f, 1.0f);
        vertices.push_back(baseCenter);

        for (int i = 0; i <= segments; ++i)
        {
            float angle = (float)i / (float)segments * 2.0f * PI;
            float x = cos(angle) * radius;
            float z = sin(angle) * radius;

            Vertex sideVertex;
            sideVertex.position = glm::vec3(x, -height * 0.5f, z);

            glm::vec3 toApex = glm::normalize(apex.position - sideVertex.position);
            glm::vec3 radial = glm::normalize(glm::vec3(x, 0.0f, z));
            sideVertex.normal = glm::normalize(glm::cross(glm::cross(toApex, radial), toApex));

            sideVertex.texCoords = glm::vec2((float)i / (float)segments, 0.0f);

            sideVertex.tangent = glm::normalize(glm::vec3(-sin(angle), 0.0f, cos(angle)));
            sideVertex.bitangent = glm::cross(sideVertex.normal, sideVertex.tangent);

            vertices.push_back(sideVertex);

            Vertex baseVertex;
            baseVertex.position = glm::vec3(x, -height * 0.5f, z);
            baseVertex.normal = glm::vec3(0.0f, -1.0f, 0.0f);
            baseVertex.texCoords = glm::vec2(
                0.5f + 0.5f * cos(angle),
                0.5f + 0.5f * sin(angle));
            baseVertex.tangent = glm::vec3(1.0f, 0.0f, 0.0f);
            baseVertex.bitangent = glm::vec3(0.0f, 0.0f, 1.0f);
            vertices.push_back(baseVertex);
        }

        for (int i = 0; i < segments; ++i)
        {
            int current = 2 + i * 2;
            int next = 2 + (i + 1) * 2;

            indices.push_back(0);
            indices.push_back(current);
            indices.push_back(next);
        }

        for (int i = 0; i < segments; ++i)
        {
            int current = 3 + i * 2;
            int next = 3 + (i + 1) * 2;

            indices.push_back(1);
            indices.push_back(next);
            indices.push_back(current);
        }
    }

    void MeshManager::generateCylinderMesh(std::vector<Vertex> &vertices, std::vector<uint32_t> &indices, int segments)
    {
        float radius = 1.0f, height = 2.0f;

        vertices.clear();
        indices.clear();

        const float PI = 3.141592653f;

        Vertex bottomCenter;
        bottomCenter.position = glm::vec3(0.0f, -height * 0.5f, 0.0f);
        bottomCenter.normal = glm::vec3(0.0f, -1.0f, 0.0f);
        bottomCenter.texCoords = glm::vec2(0.5f, 0.5f);
        bottomCenter.tangent = glm::vec3(1.0f, 0.0f, 0.0f);
        bottomCenter.bitangent = glm::vec3(0.0f, 0.0f, 1.0f);
        vertices.push_back(bottomCenter);

        Vertex topCenter;
        topCenter.position = glm::vec3(0.0f, height * 0.5f, 0.0f);
        topCenter.normal = glm::vec3(0.0f, 1.0f, 0.0f);
        topCenter.texCoords = glm::vec2(0.5f, 0.5f);
        topCenter.tangent = glm::vec3(1.0f, 0.0f, 0.0f);
        topCenter.bitangent = glm::vec3(0.0f, 0.0f, 1.0f);
        vertices.push_back(topCenter);

        for (int i = 0; i <= segments; ++i)
        {
            float angle = (float)i / (float)segments * 2.0f * PI;
            float x = cos(angle) * radius;
            float z = sin(angle) * radius;

            Vertex bottomVertex;
            bottomVertex.position = glm::vec3(x, -height * 0.5f, z);
            bottomVertex.normal = glm::vec3(0.0f, -1.0f, 0.0f);
            bottomVertex.texCoords = glm::vec2(
                0.5f + 0.5f * cos(angle),
                0.5f + 0.5f * sin(angle));
            bottomVertex.tangent = glm::vec3(1.0f, 0.0f, 0.0f);
            bottomVertex.bitangent = glm::vec3(0.0f, 0.0f, 1.0f);
            vertices.push_back(bottomVertex);

            Vertex topVertex;
            topVertex.position = glm::vec3(x, height * 0.5f, z);
            topVertex.normal = glm::vec3(0.0f, 1.0f, 0.0f);
            topVertex.texCoords = glm::vec2(
                0.5f + 0.5f * cos(angle),
                0.5f + 0.5f * sin(angle));
            topVertex.tangent = glm::vec3(1.0f, 0.0f, 0.0f);
            topVertex.bitangent = glm::vec3(0.0f, 0.0f, 1.0f);
            vertices.push_back(topVertex);

            Vertex sideBottomVertex;
            sideBottomVertex.position = glm::vec3(x, -height * 0.5f, z);
            sideBottomVertex.normal = glm::normalize(glm::vec3(x, 0.0f, z));
            sideBottomVertex.texCoords = glm::vec2((float)i / (float)segments, 0.0f);
            sideBottomVertex.tangent = glm::normalize(glm::vec3(-sin(angle), 0.0f, cos(angle)));
            sideBottomVertex.bitangent = glm::vec3(0.0f, 1.0f, 0.0f);
            vertices.push_back(sideBottomVertex);

            Vertex sideTopVertex;
            sideTopVertex.position = glm::vec3(x, height * 0.5f, z);
            sideTopVertex.normal = glm::normalize(glm::vec3(x, 0.0f, z));
            sideTopVertex.texCoords = glm::vec2((float)i / (float)segments, 1.0f);
            sideTopVertex.tangent = glm::normalize(glm::vec3(-sin(angle), 0.0f, cos(angle)));
            sideTopVertex.bitangent = glm::vec3(0.0f, 1.0f, 0.0f);
            vertices.push_back(sideTopVertex);
        }

        for (int i = 0; i < segments; ++i)
        {
            int currentBottom = 2 + i * 4;
            int nextBottom = 2 + (i + 1) * 4;
            int currentTop = 3 + i * 4;
            int nextTop = 3 + (i + 1) * 4;

            indices.push_back(0);
            indices.push_back(nextBottom);
            indices.push_back(currentBottom);

            indices.push_back(1);
            indices.push_back(currentTop);
            indices.push_back(nextTop);
        }

        for (int i = 0; i < segments; ++i)
        {
            int currentBottomSide = 4 + i * 4;
            int currentTopSide = 5 + i * 4;
            int nextBottomSide = 4 + (i + 1) * 4;
            int nextTopSide = 5 + (i + 1) * 4;

            indices.push_back(currentBottomSide);
            indices.push_back(currentTopSide);
            indices.push_back(nextBottomSide);

            indices.push_back(nextBottomSide);
            indices.push_back(currentTopSide);
            indices.push_back(nextTopSide);
        }
    }

    void MeshManager::generateSphereMesh(std::vector<Vertex> &vertices, std::vector<uint32_t> &indices, int slices,
                                         int stacks)
    {
        float radius = 1.0f;

        for (int i = 0; i <= stacks; ++i)
        {
            float phi = 3.14f * (float)i / (float)stacks;
            float y = radius * std::cos(phi);
            float ringRadius = radius * std::sin(phi);

            for (int j = 0; j <= slices; ++j)
            {
                float theta = 2.0f * 3.14f * (float)j / (float)slices;

                Vertex vertex;

                vertex.position.x = ringRadius * std::cos(theta);
                vertex.position.y = y;
                vertex.position.z = ringRadius * std::sin(theta);

                vertex.normal.x = vertex.position.x / radius;
                vertex.normal.y = vertex.position.y / radius;
                vertex.normal.z = vertex.position.z / radius;

                vertex.texCoords.x = (float)j / (float)slices;
                vertex.texCoords.y = (float)i / (float)stacks;

                vertex.tangent.x = -std::sin(theta);
                vertex.tangent.y = 0.0f;
                vertex.tangent.z = std::cos(theta);

                vertex.bitangent = glm::cross(vertex.normal, vertex.tangent);
                vertex.bitangent = glm::normalize(vertex.bitangent);

                vertices.push_back(vertex);
            }
        }

        for (int i = 0; i < stacks; ++i)
        {
            for (int j = 0; j < slices; ++j)
            {
                int first = i * (slices + 1) + j;
                int second = first + slices + 1;

                indices.push_back(first);
                indices.push_back(second);
                indices.push_back(first + 1);

                indices.push_back(second);
                indices.push_back(second + 1);
                indices.push_back(first + 1);
            }
        }
    }

    void MeshManager::generateTorusMesh(std::vector<Vertex> &vertices, std::vector<uint32_t> &indices, int slices,
                                        int stacks)
    {
        float sectorStep = 2.0f * 3.14f / (float)stacks;
        float sideStep = 2.0f * 3.14f / (float)slices;
        float minorRadius = 0.2f;
        float majorRadius = 0.5f;

        for (int i = 0; i <= slices; ++i)
        {
            float sideAngle = (float)i * sideStep;

            for (int j = 0; j <= stacks; ++j)
            {
                float sectorAngle = (float)j * sectorStep;

                Vertex vertex;

                float tubeX = minorRadius * std::cos(sideAngle);
                float tubeZ = minorRadius * std::sin(sideAngle);

                vertex.position.x = (majorRadius + tubeX) * std::cos(sectorAngle);
                vertex.position.y = (majorRadius + tubeX) * std::sin(sectorAngle);
                vertex.position.z = tubeZ;

                vertex.normal.x = std::cos(sideAngle) * std::cos(sectorAngle);
                vertex.normal.y = std::cos(sideAngle) * std::sin(sectorAngle);
                vertex.normal.z = std::sin(sideAngle);

                vertex.texCoords.x = (float)j / (float)stacks;
                vertex.texCoords.y = (float)i / (float)slices;

                vertex.tangent.x = -(majorRadius + tubeX) * std::sin(sectorAngle);
                vertex.tangent.y = (majorRadius + tubeX) * std::cos(sectorAngle);
                vertex.tangent.z = 0.0f;
                vertex.tangent = glm::normalize(vertex.tangent);

                vertex.bitangent = glm::cross(vertex.normal, vertex.tangent);
                vertex.bitangent = glm::normalize(vertex.bitangent);

                vertices.push_back(vertex);
            }
        }

        uint32_t k1, k2;
        for (int i = 0; i < slices; ++i)
        {
            k1 = i * (stacks + 1);
            k2 = k1 + stacks + 1;

            for (int j = 0; j < stacks; ++j, ++k1, ++k2)
            {
                indices.push_back(k1);
                indices.push_back(k2);
                indices.push_back(k1 + 1);

                indices.push_back(k1 + 1);
                indices.push_back(k2);
                indices.push_back(k2 + 1);
            }
        }
    }

    MeshManager *MeshManager::meshManager = nullptr;

    MeshManager *MeshManager::getMeshManager()
    {
        if (meshManager == nullptr)
            meshManager = new MeshManager();

        return meshManager;
    }
}
