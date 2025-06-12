#version 460

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoords;
layout(location = 3) in vec3 aTangent;
layout(location = 4) in vec3 aBiTangent;

out vec2 TexCoords;
out vec3 Normal;
out vec3 ViewPos;
out vec3 FragPos;
out mat3 TBN;
out vec3 WorldPos;
out vec3 skyBoxTexCoords;

uniform mat4 projection;

struct Camera {
    vec3 position;
    mat4 view;
};

uniform Camera camera;
uniform mat4 model;

uniform mat4 lightSpaceMatrix;
out vec4 FragPosLightSpace;

void main() {
    WorldPos = vec3(model * vec4(aPos, 1.0));
    
    gl_Position = projection * camera.view * vec4(WorldPos, 1.0);
    
    TexCoords = aTexCoords;
    ViewPos = camera.position;
    FragPos = WorldPos;
    
   
    mat3 normalMatrix = mat3(transpose(inverse(model)));
    
    
    vec3 T = normalize(normalMatrix * aTangent);
    vec3 B = normalize(normalMatrix * aBiTangent);
    vec3 N = normalize(normalMatrix * aNormal);
    
    
    T = normalize(T - dot(T, N) * N);
    
    
    if (dot(cross(N, T), B) < 0.0) {
        T = T * -1.0;
    }
    
    TBN = mat3(T, B, N);
    Normal = N; 

    FragPosLightSpace = lightSpaceMatrix * vec4(FragPos, 1.0);

    skyBoxTexCoords = aPos;
}