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
    float intensity;
};

uniform Light light[6];
uniform int lightCount;

uniform vec3 camPos;

struct Material
{
    sampler2D metalicTex;
    sampler2D normalTex;
};
uniform Material material;

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

vec3 GetNormalFromMap()
{
    vec3 tangentNormal = texture(material.normalTex, TexCoords).xyz * 2.0 - 1.0;
    return normalize(TBN * tangentNormal);
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

uniform vec3 aColor;
uniform float roughness;
out vec4 FragColor;

void main()
{
    vec3 viewDir = normalize(ViewPos - FragPos);

    float metallic = texture(material.metalicTex, TexCoords).r;

    vec3 N = GetNormalFromMap();
    vec3 V = normalize(camPos - WorldPos);

    vec3 F0 = vec3(0.04);
    F0 = mix(F0, aColor, metallic);

    vec3 Lo = vec3(0.0);
    for(int i=0;i<lightCount;++i)
    Lo += CalculateLight(light[i], V, N, F0, aColor, metallic, roughness);
}
