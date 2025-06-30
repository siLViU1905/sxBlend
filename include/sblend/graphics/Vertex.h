#ifndef __VERTEX_H__
#define __VERTEX_H__
#include "../../glm/glm.hpp"

struct Vertex
{
 glm::vec3 position;
 glm::vec3 normal;
 glm::vec2 texCoords;
 glm::vec3 tangent;
 glm::vec3 bitangent;

 bool operator<(const Vertex& other) const {
        if (position.x != other.position.x) return position.x < other.position.x;
        if (position.y != other.position.y) return position.y < other.position.y;
        return position.z < other.position.z;
    }
};

#endif // __VERTEX_H__