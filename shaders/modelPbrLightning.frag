#version 460 core

in vec2 TexCoords;
in vec3 Normal;
in vec3 ViewPos;
in vec3 FragPos;
in mat3 TBN;
in vec3 WorldPos;

struct Light
{
    vec3 position;
    vec3 color;
};

uniform Light light[6];
uniform int lightCount;

uniform vec3 camPos;


const float PI = 3.14159265359;

float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a = roughness * roughness;
    float a2 = a * a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH * NdotH;

    float nom = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return nom / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r * r) / 8.0;

    float nom = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return nom / denom;
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx1 = GeometrySchlickGGX(NdotV, roughness);
    float ggx2 = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}

vec3 FresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

vec3 FresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness)
{
    return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}


vec3 CalculateLight(Light light, vec3 V, vec3 N, vec3 F0, vec3 albedo, float metallic, float roughness)
{
    vec3 L = normalize(light.position - WorldPos);
    vec3 H = normalize(V + L);
    float attenuation = 1.0;

    float distance = length(light.position - WorldPos);
    attenuation = 1.0 / (distance * distance);

    float NDF = DistributionGGX(N, H, roughness);
    float G = GeometrySmith(N, V, L, roughness);
    vec3 F = FresnelSchlick(max(dot(H, V), 0.0), F0);

    vec3 numerator = NDF * G * F;
    float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001;
    vec3 specular = numerator / denominator;

    vec3 kS = F;
    vec3 kD = vec3(1.0) - kS;

    kD *= 1.0 - metallic;

    float NdotL = max(dot(N, L), 0.0);

    return (kD * albedo / PI + specular) * NdotL;
}

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
    vec4 fallbackColor;
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
};

in flat int drawID;
layout(std140, binding = 0) uniform MaterialBuffer {
    GpuMaterial materials[256];
};

uniform vec3 aColor;

out vec4 FragColor;

in vec4 FragPosLightSpace;
uniform sampler2D shadowMap;
uniform int useShadows;

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

void main()
{
    GpuMaterial mat = materials[drawID];

    vec3 albedo = texture(diffuseTexArray, vec3(TexCoords, mat.diffuseTexLayer)).rgb;
    float metallic = texture(metalnessTexArray, vec3(TexCoords, mat.metalnessTexLayer)).r;
    float roughness = texture(roughnessTexArray, vec3(TexCoords, mat.roughnessTexLayer)).r;
    float ao = texture(ambientOcclusionTexArray, vec3(TexCoords, mat.ambientOcclusionTexLayer)).r;
    vec3 emissive = texture(emissiveTexArray, vec3(TexCoords, mat.emissiveTexLayer)).rgb;
    float opacity = texture(opacityTexArray, vec3(TexCoords, mat.opacityTexLayer)).r;

    vec3 N = normalize(Normal);
    if(mat.hasNormal == 1)
    {

        vec3 tangentNormal = texture(normalTexArray, vec3(TexCoords, mat.normalTexLayer)).xyz * 2.0 - 1.0;
        N = normalize(TBN * tangentNormal);
    }

    float shadow = (useShadows == 1) ? calculateShadow(FragPosLightSpace) : 0.0;

    vec3 V = normalize(camPos - WorldPos);

    vec3 F0 = vec3(0.04);
    F0 = mix(F0, albedo, metallic);


    vec3 Lo = vec3(0.0);
    for (int i = 0; i < lightCount; ++i)
    {
        vec3 L = normalize(light[i].position - WorldPos);
        vec3 H = normalize(V + L);
        float distance = length(light[i].position - WorldPos);
        float attenuation = 1.0 / (distance * distance);
        vec3 radiance = light[i].color * attenuation;


        float NDF = DistributionGGX(N, H, roughness);
        float G = GeometrySmith(N, V, L, roughness);
        vec3 F = FresnelSchlick(max(dot(H, V), 0.0), F0);


        vec3 numerator = NDF * G * F;
        float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.001;
        vec3 specular = numerator / denominator;


        vec3 kS = F;
        vec3 kD = vec3(1.0) - kS;
        kD *= 1.0 - metallic;



        float NdotL = max(dot(N, L), 0.0);
        vec3 lightContribution = (kD * albedo / PI + specular) * radiance * NdotL;

        if (i == 0 )
            lightContribution *= (1.0 - shadow);


        Lo += lightContribution;
    }

    vec3 ambient = vec3(0.03) * albedo * ao;
    vec3 color = ambient + Lo;


    color += emissive;

    color = color / (color + vec3(1.0));

    color = pow(color, vec3(1.0 / 2.2));

    color *= (1.0 - shadow);

    FragColor = vec4(color, opacity);
}
