#ifndef __SHADER_H__
#define __SHADER_H__
#include "../../GL/glad.h"
#include "../../glm/gtc/matrix_transform.hpp"
#include "../../glm/gtc/type_ptr.hpp"
#include "../../glm/glm.hpp"

class Shader
{
    unsigned int program;
    unsigned int vertexShader;
    unsigned int fragmentShader;
    public:
    Shader();

    bool compileShader(const char* filepath, unsigned int shaderType);

    bool linkShaders();

    bool autoCompileAndLink(const char* vertexShaderFilepath, const char* fragmentShaderFilepath);

    void use();

    void setInt(const char* name, int t);

    void setFloat(const char* name, float t);

    void setVec3(const char* name, const glm::vec3& vec3);


    void setMat4(const char* name, const glm::mat4& mat4);

    bool isLocationValid(const char* name) const;

    ~Shader();
};

#endif // __SHADER_H__