#version 460 

in vec2 TexCoords;
in vec3 Normal;
in vec3 ViewPos;
in vec3 FragPos;
in mat3 TBN;

uniform sampler2DArray diffuseTexArray;
uniform sampler2DArray specularTexArray;
uniform sampler2DArray normalTexArray;
uniform sampler2DArray heightTexArray;
uniform sampler2DArray emissiveTexArray;
uniform sampler2DArray ambientOcclusionTexArray;
uniform sampler2DArray metalnessTexArray;
uniform sampler2DArray roughnessTexArray;
uniform sampler2DArray opacityTexArray;

struct GpuMaterial
{
    int diffuseTexLayer;
    int specularTexLayer;
    int normalTexLayer;
    int heightTexLayer;
    int emissiveTexLayer;
    int ambientOcclusionTexLayer;
    int metalnessTexLayer;
    int roughnessTexLayer;
    int opacityTexLayer;
    int hasDiffuse;
    int hasSpecular;
    int hasNormal;
    int hasHeight;
    int hasEmissive;
    int hasAmbientOcclusion;
    int hasMetalness;
    int hasRoughness;
    int hasOpacity;
    vec4 fallbackColor;
};

in flat int drawID;
layout(std140, binding = 0) uniform MaterialBuffer
{
    GpuMaterial materials[256];
};

out vec4 FragColor;


in vec4 FragPosLightSpace;
uniform sampler2D shadowMap;

struct Light
{
    vec3 position;
};
uniform Light light;

float calculateShadow(vec4 fpls)
{
    vec3 projCoords = fpls.xyz / fpls.w;

    projCoords = projCoords * 0.5 + 0.5;

    if (projCoords.x < 0.0 || projCoords.x > 1.0 ||
    projCoords.y < 0.0 || projCoords.y > 1.0 ||
    projCoords.z < 0.0 || projCoords.z > 1.0)
    return 0.0;

    float currentDepth = projCoords.z;

    vec3 lightDir = normalize(light.position - FragPos);
    float bias = max(0.05 * (1.0 - dot(Normal, lightDir)), 0.005);

    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    for (int x = -3; x <= 3; ++x)
    {
        for (int y = -3; y <= 3; ++y)
        {
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
            shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
        }
    }
    shadow /= 49.0;

    return shadow;
}

uniform int useShadows;


void main()
{
    GpuMaterial currentMaterial = materials[drawID];

    vec3 diffuseColor = currentMaterial.fallbackColor.rgb;
    vec3 specularColor = vec3(0.0);

    if (currentMaterial.hasDiffuse == 1)
    diffuseColor = texture(diffuseTexArray, vec3(TexCoords, currentMaterial.diffuseTexLayer)).rgb;

    if (currentMaterial.hasSpecular == 1)
    specularColor = texture(specularTexArray, vec3(TexCoords, currentMaterial.specularTexLayer)).rgb;


    float shadow = 0.0;

    if (useShadows == 1)
    shadow = calculateShadow(FragPosLightSpace);

    vec3 result = diffuseColor + specularColor;


    result *= (1.0 - shadow);

    FragColor = vec4(result, 1.0);
}