#include "PhysicsWorld.h"
#include "../../GLFW/glfw3.h"
#define GLM_ENABLE_EXPERIMENTAL
#include "../../glm/gtx/matrix_decompose.hpp"
#include "../../glm/gtx/norm.hpp"

namespace sx
{
    bool Physics::AABB::intersects(const AABB &other) const
    {
        return (min.x <= other.max.x && max.x >= other.min.x) &&
               (min.y <= other.max.y && max.y >= other.min.y) &&
               (min.z <= other.max.z && max.z >= other.min.z);
    }

    Physics::AABB Physics::calculateWorldAABB(Mesh &mesh)
    {
        auto model = mesh.getModel();
        const auto &vertices = mesh.getVertices();
        if (vertices.empty())
            return AABB{glm::vec3(0), glm::vec3(0)};

        glm::vec3 min = glm::vec3(model * glm::vec4(vertices[0].position, 1.0f));
        glm::vec3 max = min;

        for (const auto &vertex : vertices)
        {
            glm::vec3 worldPos = glm::vec3(model * glm::vec4(vertex.position, 1.0f));
            min = glm::min(min, worldPos);
            max = glm::max(max, worldPos);
        }

        return AABB{min, max};
    }

    bool Physics::OBB::intersects(const OBB &obb) const
    {
        glm::vec3 a_u[3] = {rotation[0], rotation[1], rotation[2]};
        glm::vec3 b_u[3] = {obb.rotation[0], obb.rotation[1], obb.rotation[2]};

        glm::vec3 d = obb.center - center;

        float R[3][3];
        for (int i = 0; i < 3; ++i)
            for (int j = 0; j < 3; ++j)
                R[i][j] = glm::dot(a_u[i], b_u[j]);

        float extentA, extentB, separation;

        for (int i = 0; i < 3; ++i)
        {
            extentA = extents[i];
            extentB = obb.extents[0] * std::abs(R[i][0]) +
                      obb.extents[1] * std::abs(R[i][1]) +
                      obb.extents[2] * std::abs(R[i][2]);
            separation = std::abs(glm::dot(d, a_u[i]));
            if (separation > extentA + extentB)
                return false;
        }

        // 2. Axele lui B
        for (int i = 0; i < 3; ++i)
        {
            extentA = extents[0] * std::abs(R[0][i]) +
                      extents[1] * std::abs(R[1][i]) +
                      extents[2] * std::abs(R[2][i]);
            extentB = obb.extents[i];
            separation = std::abs(glm::dot(d, b_u[i]));
            if (separation > extentA + extentB)
                return false;
        }

        // 3. Produsele cross între axe
        for (int i = 0; i < 3; ++i)
        {
            for (int j = 0; j < 3; ++j)
            {
                glm::vec3 axis = glm::cross(a_u[i], b_u[j]);
                if (glm::length2(axis) < 0.001f)
                    continue; // Axe paralele

                axis = glm::normalize(axis);
                extentA = extents[0] * std::abs(glm::dot(axis, a_u[0])) +
                          extents[1] * std::abs(glm::dot(axis, a_u[1])) +
                          extents[2] * std::abs(glm::dot(axis, a_u[2]));
                extentB = obb.extents[0] * std::abs(glm::dot(axis, b_u[0])) +
                          obb.extents[1] * std::abs(glm::dot(axis, b_u[1])) +
                          obb.extents[2] * std::abs(glm::dot(axis, b_u[2]));
                separation = std::abs(glm::dot(d, axis));
                if (separation > extentA + extentB)
                    return false;
            }
        }

        return true;
    }

    Physics::OBB Physics::calculateOBB(Mesh &mesh)
    {
        glm::mat4 model = mesh.getModel();

        glm::vec3 scale;
        glm::quat rotation;
        glm::vec3 translation;
        glm::vec3 skew;
        glm::vec4 perspective;
        glm::decompose(model, scale, rotation, translation, skew, perspective);

        OBB obb;
        obb.center = translation;
        obb.extents = scale * 0.5f;
        obb.rotation = glm::mat3_cast(rotation);

        return obb;
    }

    glm::vec3 Physics::calculateCollisionCorrection(const OBB &a, const OBB &b)
    {
        float overlapY = (a.center.y - a.extents.y) - (b.center.y + b.extents.y);
        return glm::vec3(0.0f, -overlapY, 0.0f);
    }

    void Physics::applyGravity(Mesh &mesh, Mesh &gridMesh)
    {
        glm::mat4 model = mesh.getModel();

        AABB meshAABB = calculateWorldAABB(mesh);
        float meshBottomY = meshAABB.min.y;

        if (meshBottomY > gridMesh.getPosition().y)
            mesh.move(gravity * deltaTime);
        else if (meshBottomY < gridMesh.getPosition().y)
        {
            glm::vec3 newPos = mesh.getPosition();
            newPos.y = gridMesh.getPosition().y + (meshAABB.max.y - meshAABB.min.y) / 2.f;
            mesh.setPosition(newPos);
        }
    }

    void Physics::applyMeshCollision(Mesh &a, Mesh &b)
    {
        AABB aAABB = calculateWorldAABB(a);
        AABB bAABB = calculateWorldAABB(b);

        if (aAABB.intersects(bAABB))
        {
            glm::vec3 overlap;
            overlap.x = std::min(aAABB.max.x, bAABB.max.x) - std::max(aAABB.min.x, bAABB.min.x);
            overlap.y = std::min(aAABB.max.y, bAABB.max.y) - std::max(aAABB.min.y, bAABB.min.y);
            overlap.z = std::min(aAABB.max.z, bAABB.max.z) - std::max(aAABB.min.z, bAABB.min.z);

            if (overlap.x < overlap.y && overlap.x < overlap.z)
                handleCollisionX(a, b);

            else if (overlap.y < overlap.z)
                handleCollisionY(a, b);

            else
                handleCollisionZ(a, b);
        }

        a.move(a.velocity * deltaTime);
        b.move(b.velocity * deltaTime);
    }

    void Physics::handleCollisionX(Mesh &a, Mesh &b)
    {
        float tempVx = a.velocity.x;
        a.velocity.x = b.velocity.x * b.mass / a.mass;
        b.velocity.x = tempVx * a.mass / b.mass;

        a.velocity.x *= 0.9f;
        b.velocity.x *= 0.9f;
    }

    void Physics::handleCollisionY(Mesh &a, Mesh &b)
    {
        float tempVy = a.velocity.y;
        a.velocity.y = b.velocity.y * b.mass / a.mass;
        b.velocity.y = tempVy * a.mass / b.mass;
    }

    void Physics::handleCollisionZ(Mesh &a, Mesh &b)
    {
        float tempVz = a.velocity.z;
        a.velocity.z = b.velocity.z * b.mass / a.mass;
        b.velocity.z = tempVz * a.mass / b.mass;
    }

    void Physics::applyDrag(Mesh &mesh)
    {
        const float dragCoefficient = 0.5f;
        mesh.velocity *= (1.0f - dragCoefficient * deltaTime);

        if (glm::length(mesh.velocity) < 0.001f)
            mesh.velocity = glm::vec3(0.0f);
    }

    void Physics::startSimulation()
    {
        isRunning = true;
    }

    void Physics::stopSimulation()
    {
        isRunning = false;
    }

    void Physics::setGrid(Mesh &gridMesh)
    {
        gridAABB = calculateWorldAABB(gridMesh);
    }

    Physics::Physics()
    {
    }
}