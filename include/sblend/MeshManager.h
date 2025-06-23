#ifndef __MESHMANAGER_H__
#define __MESHMANAGER_H__
#include "graphics/Mesh.h"
#include "Menu.h"

namespace sx
{

    class MeshManager
    {
        std::vector<Mesh> meshes;

        std::vector<std::pair<glm::vec3, glm::vec3>> initialMeshesPositionsAndVelocities;

        static MeshManager *meshManager;

        MeshManager();

        static void calculateTangentBitangent(const Vertex& v0, const Vertex& v1, const Vertex& v2,glm::vec3& tangent, glm::vec3& bitangent);

        static void calculateTangentSpace(std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices);

        static void generateGridLines(std::vector<Vertex> &vertices, std::vector<uint32_t> &indices);

        static void generatePlaneMesh(std::vector<Vertex> &vertices, std::vector<uint32_t> &indices);

        static void generateCircleMesh(std::vector<Vertex> &vertices, std::vector<uint32_t> &indices, int segments = 30);

        static void generateCubeMesh(std::vector<Vertex> &vertices, std::vector<uint32_t> &indices);

        static void generateConeMesh(std::vector<Vertex>& vertices, std::vector<uint32_t>& indices, int segments = 32);

        static void generateCylinderMesh(std::vector<Vertex>& vertices, std::vector<uint32_t>& indices, int segments = 32);

        static void generateSphereMesh(std::vector<Vertex> &vertices, std::vector<uint32_t> &indices, int slices = 32, int stacks = 16);

        static void generateTorusMesh(std::vector<Vertex> &vertices, std::vector<uint32_t> &indices, int slices = 32, int stacks = 16);

    public:
        static MeshManager *getMeshManager();

        friend class Application;
    };

}

#endif // __MESHMANAGER_H__