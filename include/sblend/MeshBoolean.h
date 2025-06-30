//
// Created by Silviu on 28.06.2025.
//

#ifndef CSG_H
#define CSG_H

#include "graphics/Mesh.h"
#include "../manifold/manifold.h"
#include "../manifold/meshIO.h"
#include "graphics/Vertex.h"

namespace sx
{
    enum class MeshBooleanOperation
    {
        UNION,
        SUBTRACT,
        INTERSECT
    };

    class MeshBoolean
    {
        static void manifoldToMesh(const manifold::Manifold& manifold, std::vector<Vertex>& vertices, std::vector<uint32_t>& indices);

        static manifold::Manifold createManifoldFromMesh(const Mesh& mesh);

        static void applyMeshTransforms(manifold::Manifold& manifold, const Mesh& mesh);

        static void calculateNormals(std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices);

        static void calculateTexCoords(std::vector<Vertex>& vertices);

        static void calculateTangentsAndBitangents(std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices);

        static manifold::Manifold meshToManifold(const Mesh& mesh);
    public:

        static void performOperation(Mesh& meshA, const Mesh& meshB, MeshBooleanOperation operation, std::vector<Vertex>& vertices, std::vector<uint32_t>& indices);
    };
}


#endif //CSG_H
