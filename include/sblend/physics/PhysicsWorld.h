#ifndef __PHYSICSWORLD_H__
#define __PHYSICSWORLD_H__
#include "../graphics/Mesh.h"

namespace sx
{
    class Physics
    {

        struct AABB
        {
            glm::vec3 min;
            glm::vec3 max;

            bool intersects(const AABB &other) const;
        };

        AABB calculateWorldAABB(Mesh &mesh);

        struct OBB
        {
            glm::vec3 center;
            glm::vec3 extents; 
            glm::mat3 rotation;

            bool intersects(const OBB& obb) const;
        };

        OBB calculateOBB(Mesh& mesh);

        glm::vec3 calculateCollisionCorrection(const OBB& a, const OBB& b);

        glm::vec3 gravity;

        void applyGravity(Mesh &mesh, Mesh& gridMesh);

        void applyMeshCollision(Mesh& a, Mesh& b);

        void handleCollisionX(Mesh& a, Mesh& b);

        void handleCollisionY(Mesh& a, Mesh& b);

        void handleCollisionZ(Mesh& a, Mesh& b);

        void applyDrag(Mesh& mesh);

        float deltaTime;

        bool isRunning = false;

        void startSimulation();

        void stopSimulation();

        AABB gridAABB;

        void setGrid(Mesh &gridMesh);

    public:
        Physics();

        friend class Application;
    };
}

#endif // __PHYSICSWORLD_H__