#include "Mesh.h"
#include "../../stb_image.h"

std::unordered_map<MeshType, std::string> Mesh::MeshTypesMap;
std::unordered_map<std::string, MeshType> Mesh::MeshStringMap;

Mesh::Mesh(const std::vector<Vertex> &_vertices, const std::vector<uint32_t> &_indices, GLenum renderMode) : vertices(_vertices), indices(_indices),
                                                                                                             renderMode(renderMode)
{
    position = glm::vec3(0.f, 1.f, 0.f);
    velocity = angles = glm::vec3(0.f);
    scale = glm::vec3(0.5f);
    color = glm::vec3(0.8f);
    mass = 1.f;

    metallic = roughness = ao = 0.5f;

    isReflective = false;

    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);
    glGenVertexArrays(1, &vao);

    glBindVertexArray(vao);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(uint32_t), indices.data(), GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, normal));

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, texCoords));

    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, tangent));

    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, bitangent));

    glBindVertexArray(0);
}

Mesh::Mesh(const std::vector<Vertex> &_vertices, const std::vector<uint32_t> &_indices, const std::vector<Texture> &_textures) : vertices(_vertices), indices(_indices),
                                                                                                                                 textures(_textures)
{
    position = glm::vec3(0.f, 1.f, 0.f);
    velocity = angles = glm::vec3(0.f);
    scale = glm::vec3(0.5f);
    color = glm::vec3(0.8f);
    mass = 1.f;

    metallic = roughness = ao = 0.5f;

    renderMode = GL_TRIANGLES;

    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);
    glGenVertexArrays(1, &vao);

    glBindVertexArray(vao);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(uint32_t), indices.data(), GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, normal));

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, texCoords));

    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, tangent));

    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, bitangent));

    glBindVertexArray(0);
}

void Mesh::setVertices(const std::vector<Vertex> &_vertices)
{

    vertices = _vertices;

    glBindVertexArray(vao);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, normal));

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, texCoords));

    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, tangent));

    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, bitangent));

    glBindVertexArray(0);
}

void Mesh::setIndices(const std::vector<uint32_t> &_indices)
{
    indices = _indices;

    glBindVertexArray(vao);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(uint32_t), indices.data(), GL_STATIC_DRAW);

    glBindVertexArray(0);
}

void Mesh::render(Shader &shader)
{
    model = glm::mat4(1.f);
    model = glm::translate(model, position);
    model = glm::rotate(model, glm::radians(angles.x), glm::vec3(1.f, 0.f, 0.f));
    model = glm::rotate(model, glm::radians(angles.y), glm::vec3(0.f, 1.f, 0.f));
    model = glm::rotate(model, glm::radians(angles.z), glm::vec3(0.f, 0.f, 1.f));
    model = glm::scale(model, scale);
    mass = 0.6666f * (scale.x + scale.y + scale.z);

    shader.setMat4("model", model);
    shader.setVec3("aColor", color);

    shader.setFloat("metallic", metallic);
    shader.setFloat("roughness", roughness);
    shader.setFloat("ao", ao);
    shader.setInt("hasTex", 0);

    if (hasTexture)
    {
        shader.setInt("hasTex", 1);

        glActiveTexture(GL_TEXTURE0);

        shader.setInt("_texture", 0);
        glBindTexture(GL_TEXTURE_2D, texture);
    }

    glBindVertexArray(vao);

    glDrawElements(renderMode, indices.size(), GL_UNSIGNED_INT, nullptr);

    glBindVertexArray(0);
}

void Mesh::renderForModelUse(Shader &shader)
{
    shader.setFloat("material.shininess", 30.f);

    for (int i = 0; i < textures.size(); ++i)
    {
        glActiveTexture(GL_TEXTURE0 + i);

        if (textures[i].type == "normalTex")
            shader.setInt("hasNormalTex", 1);
        else
            shader.setInt("hasNormalTex", 0);

        if (textures[i].type == "heightTex")
            shader.setInt("hasHeightTex", 1);
        else
            shader.setInt("hasHeightTex", 0);

        shader.setInt(("material." + textures[i].type).c_str(), i);
        glBindTexture(GL_TEXTURE_2D, textures[i].id);
    }

    glActiveTexture(GL_TEXTURE0);

    glBindVertexArray(vao);

    glDrawElements(renderMode, indices.size(), GL_UNSIGNED_INT, nullptr);

    glBindVertexArray(0);
}

const glm::mat4 &Mesh::getModel()
{
    model = glm::mat4(1.f);
    model = glm::translate(model, position);
    model = glm::rotate(model, glm::radians(angles.x), glm::vec3(1.f, 0.f, 0.f));
    model = glm::rotate(model, glm::radians(angles.y), glm::vec3(0.f, 1.f, 0.f));
    model = glm::rotate(model, glm::radians(angles.z), glm::vec3(0.f, 0.f, 1.f));
    model = glm::scale(model, scale);

    return model;
}

void Mesh::getProperties(std::ostringstream &stream)
{
    stream.clear();
    stream.str("");

    stream << MeshTypesMap[type] << '\n';
    stream << slices << '\n';
    stream << stacks << '\n';
    stream << position.x << ' ' << position.y << ' ' << position.z << '\n';
    stream << velocity.x << ' ' << velocity.y << ' ' << velocity.z << '\n';
    stream << scale.x << ' ' << scale.y << ' ' << scale.z << '\n';
    stream << angles.x << ' ' << angles.y << ' ' << angles.z << '\n';
    stream << color.x << ' ' << color.y << ' ' << color.z << '\n';
    stream << metallic << '\n';
    stream << roughness << '\n';
    stream << ao << '\n';
}

void Mesh::applyTexture(const char *filepath)
{
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int width, height, channels;
    unsigned char *pixels = stbi_load(filepath, &width, &height, &channels, 0);

    if (pixels)
    {
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

        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, pixels);
        glGenerateMipmap(GL_TEXTURE_2D);

        hasTexture = true;
    }

    stbi_image_free(pixels);
}

Mesh::~Mesh()
{
    /*glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &ebo);
    glDeleteVertexArrays(1, &vao);*/
}

const std::vector<Vertex> &Mesh::getVertices() const
{
    return vertices;
}

const glm::vec3 &Mesh::getPosition() const
{
    return position;
}

void Mesh::setPosition(const glm::vec3 &position)
{
    this->position = position;
}

void Mesh::move(const glm::vec3 &offset)
{
    position += offset;
}
