#include "Light.h"
#include <string>

namespace sx
{
    std::unordered_map<LightType, std::string> Light::LightTypesMap = {
        {LightType::POINT, "POINT"},
        {LightType::DIRECTIONAL, "DIRECTIONAL"},
        {LightType::SPOT, "SPOT"},
        {LightType::PBR, "PBR"}
    };

    std::unordered_map<std::string, LightType> Light::LightStringMap = {
        {"POINT", LightType::POINT},
        {"DIRECTIONAL", LightType::DIRECTIONAL},
        {"SPOT", LightType::SPOT},
        {"PBR", LightType::PBR}
    };

    Light::Light(LightType type) : type(type)
    {
        direction = position = glm::vec3(0.f);
        direction.z = -1.f;
        ambient = specular = color = diffuse = glm::vec3(1.f);
        constant = 1.f;
        linear = 0.2f;
        quadratic = 0.032f;
        cutOff = 12.5f;
        outerCutOff = 15.f;
        intensity = 1.f;
        view = glm::lookAt(position, glm::vec3(0.f), glm::vec3(0.f, 1.f, 0.f));
        projection = glm::ortho(-5.f, 5.f, -5.f, 5.f, 0.1f, 20.f);
    }

    void Light::apply_to_shader(Shader &shader, int index)
    {
        switch (type)
        {
            case LightType::POINT:
                shader.setVec3(("light[" + std::to_string(index) + "].position").c_str(), position);
                shader.setVec3(("light[" + std::to_string(index) + "].diffuse").c_str(), diffuse);
                shader.setVec3(("light[" + std::to_string(index) + "].specular").c_str(), specular);
                shader.setVec3(("light[" + std::to_string(index) + "].ambient").c_str(), ambient);
                shader.setVec3(("light[" + std::to_string(index) + "].color").c_str(), color);
                shader.setFloat(("light[" + std::to_string(index) + "].linear").c_str(), linear);
                shader.setFloat(("light[" + std::to_string(index) + "].constant").c_str(), constant);
                shader.setFloat(("light[" + std::to_string(index) + "].quadratic").c_str(), quadratic);
                shader.setInt(("light[" + std::to_string(index) + "].type").c_str(), (int) type);
                view = glm::lookAt(position, glm::vec3(0.f), glm::vec3(0.f, 1.f, 0.f));
                shader.setMat4(("lightView[" + std::to_string(index) + "]").c_str(), view);
                shader.setMat4(("lightProjection[" + std::to_string(index) + "]").c_str(), projection);
                break;

            case LightType::DIRECTIONAL:
                shader.setVec3(("light[" + std::to_string(index) + "].position").c_str(), position);
                shader.setVec3(("light[" + std::to_string(index) + "].direction").c_str(), direction);
                shader.setVec3(("light[" + std::to_string(index) + "].diffuse").c_str(), diffuse);
                shader.setVec3(("light[" + std::to_string(index) + "].specular").c_str(), specular);
                shader.setVec3(("light[" + std::to_string(index) + "].ambient").c_str(), ambient);
                shader.setVec3(("light[" + std::to_string(index) + "].color").c_str(), color);
                shader.setInt(("light[" + std::to_string(index) + "].type").c_str(), (int) type);
                view = glm::lookAt(position, glm::vec3(0.f), glm::vec3(0.f, 1.f, 0.f));
                shader.setMat4(("lightView[" + std::to_string(index) + "]").c_str(), view);
                shader.setMat4(("lightProjection[" + std::to_string(index) + "]").c_str(), projection);
                break;

            case LightType::SPOT:
                shader.setVec3(("light[" + std::to_string(index) + "].position").c_str(), position);
                shader.setVec3(("light[" + std::to_string(index) + "].direction").c_str(), direction);
                shader.setVec3(("light[" + std::to_string(index) + "].diffuse").c_str(), diffuse);
                shader.setVec3(("light[" + std::to_string(index) + "].specular").c_str(), specular);
                shader.setVec3(("light[" + std::to_string(index) + "].ambient").c_str(), ambient);
                shader.setVec3(("light[" + std::to_string(index) + "].color").c_str(), color);
                shader.setFloat(("light[" + std::to_string(index) + "].linear").c_str(), linear);
                shader.setFloat(("light[" + std::to_string(index) + "].constant").c_str(), constant);
                shader.setFloat(("light[" + std::to_string(index) + "].quadratic").c_str(), quadratic);
                shader.setFloat(("light[" + std::to_string(index) + "].cutOff").c_str(),
                                glm::cos(glm::radians(cutOff)));
                shader.setFloat(("light[" + std::to_string(index) + "].outerCutOff").c_str(),
                                glm::cos(glm::radians(outerCutOff)));
                shader.setFloat(("light[" + std::to_string(index) + "].intensity").c_str(), intensity);
                shader.setInt(("light[" + std::to_string(index) + "].type").c_str(), (int) type);
                view = glm::lookAt(position, glm::vec3(0.f), glm::vec3(0.f, 1.f, 0.f));
                shader.setMat4(("lightView[" + std::to_string(index) + "]").c_str(), view);
                shader.setMat4(("lightProjection[" + std::to_string(index) + "]").c_str(), projection);
                break;

            case LightType::PBR:
                shader.setVec3(("light[" + std::to_string(index) + "].position").c_str(), position);
                shader.setVec3(("light[" + std::to_string(index) + "].color").c_str(), color);
                view = glm::lookAt(position, glm::vec3(0.f), glm::vec3(0.f, 1.f, 0.f));
                shader.setMat4(("lightView[" + std::to_string(index) + "]").c_str(), view);
                shader.setMat4(("lightProjection[" + std::to_string(index) + "]").c_str(), projection);
                break;
        }
    }

    void Light::getProperties(std::ostringstream &stream) const
    {
        stream.clear();
        stream.str("");

        stream << LightTypesMap[type] << '\n';
        stream << position.x << ' ' << position.y << ' ' << position.z << '\n';
        stream << direction.x << ' ' << direction.y << ' ' << direction.z << '\n';
        stream << diffuse.x << ' ' << diffuse.y << ' ' << diffuse.z << '\n';
        stream << specular.x << ' ' << specular.y << ' ' << specular.z << '\n';
        stream << ambient.x << ' ' << ambient.y << ' ' << ambient.z << '\n';
        stream << color.x << ' ' << color.y << ' ' << color.z << '\n';
        stream << linear << '\n';
        stream << constant << '\n';
        stream << quadratic << '\n';
        stream << cutOff << '\n';
        stream << outerCutOff << '\n';
        stream << intensity <<'\n';
    }
}
