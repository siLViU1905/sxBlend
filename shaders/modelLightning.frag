#version 460

in vec2 TexCoords;
in vec3 Normal;
in vec3 ViewPos;
in vec3 FragPos;
in mat3 TBN;

out vec4 FragColor;

struct Light {
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    vec3 color;
    float constant;
    float linear;
    float quadratic;
};
uniform Light light[6];
uniform int lightCount;

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
layout(std140, binding = 0) uniform MaterialBuffer {
    GpuMaterial materials[256];
};

in vec4 FragPosLightSpace;
uniform sampler2D shadowMap;
uniform int useShadows;

vec3 calculateDiffuse(vec3 normal, vec3 lightDir, vec3 lightDiffuse, vec3 materialDiffuse) {
    float diff = max(dot(normal, lightDir), 0.0);
    return diff * lightDiffuse * materialDiffuse;
}

vec3 calculateSpecular(vec3 normal, vec3 lightDir, vec3 viewDir, vec3 lightSpecular, vec3 materialSpecular, float shininess) {
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
    return spec * lightSpecular * materialSpecular;
}

float calculateAttenuation(float distance, float constant, float linear, float quadratic) {
    return 1.0 / (constant + linear * distance + quadratic * (distance * distance));
}

vec2 calculateParallaxCoords(vec3 viewDir, float height) {
    const float heightScale = 0.1;
    vec2 p = viewDir.xy * (height * heightScale);
    return TexCoords - p;
}

float calculateShadow(vec4 fragPosLightSpace) {
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;

    if (projCoords.z > 1.0) return 0.0;

    float currentDepth = projCoords.z;
    vec3 lightDir = normalize(light[0].position - FragPos);
    float bias = max(0.05 * (1.0 - dot(Normal, lightDir)), 0.005);

    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    for (int x = -1; x <= 1; ++x) {
        for (int y = -1; y <= 1; ++y) {
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
            shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
        }
    }
    shadow /= 9.0;
    return shadow;
}

void main() {
    GpuMaterial mat = materials[drawID];

    vec2 pTexCoords = TexCoords;
    vec3 viewDirTangent = transpose(TBN) * normalize(ViewPos - FragPos);
    if (mat.hasHeight == 1 && mat.hasNormal == 1)
    {
        float height = texture(heightTexArray, vec3(TexCoords, mat.heightTexLayer)).r;
        pTexCoords = calculateParallaxCoords(viewDirTangent, height);
    }


    vec3 materialDiffuse = texture(diffuseTexArray, vec3(pTexCoords, mat.diffuseTexLayer)).rgb;
    vec3 materialSpecular = texture(specularTexArray, vec3(pTexCoords, mat.specularTexLayer)).rgb;
    float shininess = 32.0;

    float ao = 1.0;
    if (mat.hasAmbientOcclusion == 1) {
        ao = texture(ambientOcclusionTexArray, vec3(pTexCoords, mat.ambientOcclusionTexLayer)).r;
    }

    vec3 normal = normalize(Normal);
    if (mat.hasNormal == 1) {
        vec3 tangentNormal = texture(normalTexArray, vec3(pTexCoords, mat.normalTexLayer)).xyz * 2.0 - 1.0;
        normal = normalize(TBN * tangentNormal);
    }

    vec3 emissive = vec3(0.0);
    if (mat.hasEmissive == 1) {
        emissive = texture(emissiveTexArray, vec3(pTexCoords, mat.emissiveTexLayer)).rgb;
    }


    vec3 viewDir = normalize(ViewPos - FragPos);
    vec3 totalLighting = vec3(0.0);

    float shadow = (useShadows == 1) ? calculateShadow(FragPosLightSpace) : 0.0;

    for (int i = 0; i < lightCount; i++)
    {
        float distance = length(light[i].position - FragPos);
        float attenuation = calculateAttenuation(distance, light[i].constant, light[i].linear, light[i].quadratic);

        vec3 lightDir = normalize(light[i].position - FragPos);

        vec3 ambient = light[i].ambient * materialDiffuse * ao;

        vec3 diffuse = calculateDiffuse(normal, lightDir, light[i].diffuse, materialDiffuse);


        vec3 specular = calculateSpecular(normal, lightDir, viewDir, light[i].specular, materialSpecular, shininess);

        if (i == 0) {
            totalLighting += (ambient + (1.0 - shadow) * (diffuse + specular)) * light[i].color * attenuation;
        } else {
            totalLighting += (ambient + diffuse + specular) * light[i].color * attenuation;
        }
    }


    vec3 finalColor = totalLighting + emissive;


    float opacity = 1.0;
    if (mat.hasOpacity == 1) {
        opacity = texture(opacityTexArray, vec3(pTexCoords, mat.opacityTexLayer)).r;
    }

    FragColor = vec4(finalColor, opacity);
}