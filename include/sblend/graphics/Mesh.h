#ifndef __MESH_H__
#define __MESH_H__
#include "Vertex.h"
#include "Shader.h"
#include <sstream>
#include <unordered_map>

enum class MeshType
{
    NaN,
    PLANE,
    CIRCLE,
    CUBE,
    CONE,
    CYLINDER,
    SPHERE,
    TORUS
};

namespace std
{
    template<>
    struct hash<MeshType>
    {
        size_t operator()(const MeshType &mt) const
        {
            return static_cast<size_t>(mt);
        }
    };
}

struct Texture
{
    uint32_t id;
    std::string type;

    bool load(const char *filepath, const std::string &type);
    bool loadCubemap(const std::vector<std::string>& faces);
};

class Mesh
{
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;


    glm::mat4 model;

    GLenum renderMode;

    void calculateBounidngSphere();
public:
    Mesh(const std::vector<Vertex> &_vertices, const std::vector<uint32_t> &_indices, GLenum renderMode = GL_TRIANGLES);

    Mesh(const std::vector<Vertex> &_vertices, const std::vector<uint32_t> &_indices,
         const std::vector<Texture> &_textures);

    static std::unordered_map<MeshType, std::string> MeshTypesMap;

    static std::unordered_map<std::string, MeshType> MeshStringMap;

    void setVertices(const std::vector<Vertex> &_vertices);

    void setIndices(const std::vector<uint32_t> &_indices);

    void render(Shader &shader);

    void renderForSkyboxUse(Shader& shader);

    void renderForTerrainUse(Shader& shader);

    std::vector<Texture> textures;

    int slices = 32, stacks = 16;

    MeshType type;

    glm::vec3 position;
    glm::vec3 scale;
    glm::vec3 angles;
    glm::vec3 color;

    float metallic;
    float roughness;
    float ao;

    bool hasTexture = false;

    bool isReflective;

    int useFlatReflection;

    uint32_t vbo, vao, ebo;
    uint32_t texture = 0;

    const glm::mat4 &getModel();

    void getProperties(std::ostringstream &stream);

    void applyTexture(const char *filepath);

    ~Mesh();

    float mass;

    glm::vec3 velocity;

    const std::vector<Vertex> &getVertices() const;

    const std::vector<uint32_t>& getIndices() const;

    const glm::vec3 &getPosition() const;

    glm::vec3 boundingSphereCenter;
    float boundingSphereRadius;


    void setPosition(const glm::vec3 &position);

    void move(const glm::vec3 &offset);

    friend class MeshManager;
    friend class Physics;
};
#endif // __MESH_H__
