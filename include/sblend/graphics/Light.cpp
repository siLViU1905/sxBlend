#include "Light.h"
#include <string>



namespace sx
{
    std::unordered_map<LightType, std::string> Light::LightTypesMap = {
        {LightType::POINT, "POINT"},
        {LightType::PBR, "PBR"}};

    std::unordered_map<std::string, LightType> Light::LightStringMap = {
        {"POINT", LightType::POINT},
        {"PBR", LightType::PBR}};

    Light::Light(LightType type) : type(type)
    {
        direction = position = glm::vec3(0.f);
        ambient = specular = color = diffuse = glm::vec3(1.f);
        constant = 1.f;
        linear = 0.2f;
        quadratic = 0.032f;
    }

    void Light::apply_to_shader(Shader &shader, int index)
    {
        if (type == LightType::POINT)
        {
            shader.setVec3(("light[" + std::to_string(index) + "].position").c_str(), position);
            shader.setVec3(("light[" + std::to_string(index) + "].diffuse").c_str(), diffuse);
            shader.setVec3(("light[" + std::to_string(index) + "].specular").c_str(), specular);
            shader.setVec3(("light[" + std::to_string(index) + "].ambient").c_str(), ambient);
            shader.setVec3(("light[" + std::to_string(index) + "].color").c_str(), color);
            shader.setFloat(("light[" + std::to_string(index) + "].linear").c_str(), linear);
            shader.setFloat(("light[" + std::to_string(index) + "].constant").c_str(), constant);
            shader.setFloat(("light[" + std::to_string(index) + "].quadratic").c_str(), quadratic);
        }
        else
        {
            shader.setVec3(("light[" + std::to_string(index) + "].position").c_str(), position);
            shader.setVec3(("light[" + std::to_string(index) + "].color").c_str(), color);
        }
    }
    
    void Light::getProperties(std::ostringstream &stream) const
    {
        stream.clear();
        stream.str("");

        stream << LightTypesMap[type] << '\n';
        stream << position.x << ' ' << position.y << ' ' << position.z << '\n';
        stream << diffuse.x << ' ' << diffuse.y << ' ' << diffuse.z << '\n';
        stream << specular.x << ' ' << specular.y << ' ' << specular.z << '\n';
        stream << ambient.x << ' ' << ambient.y << ' ' << ambient.z << '\n';
        stream << color.x << ' ' << color.y << ' ' << color.z << '\n';
        stream << linear << '\n';
        stream << constant << '\n';
        stream << quadratic << '\n';
    }

}