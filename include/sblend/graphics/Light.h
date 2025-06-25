#pragma once
#include "Mesh.h"

enum class LightType
{
    NaN,
    POINT,
    DIRECTIONAL,
    SPOT,
    PBR
};

namespace std
{
    template <>
    struct hash<LightType>
    {
        size_t operator()(const LightType &type) const
        {
            return static_cast<size_t>(type);
        }
    };
}

namespace sx
{

    class Light
    {
    public:
        static std::unordered_map<LightType, std::string> LightTypesMap;
        static std::unordered_map<std::string, LightType> LightStringMap;

        glm::vec3 position;
        glm::vec3 direction;
        glm::vec3 diffuse;
        glm::vec3 specular;
        glm::vec3 ambient;
        glm::vec3 color;

        LightType type;

        float constant;
        float linear;
        float quadratic;
        float cutOff;
        float outerCutOff;

        glm::mat4 view, projection;

        Light(LightType type);

        void apply_to_shader(Shader &shader, int index = 0);

        void getProperties(std::ostringstream &oss) const;
    };

}