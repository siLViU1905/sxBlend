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

uniform vec3 albedoColor;
uniform float metallic;
uniform float roughness;
uniform float ao;

uniform vec3 camPos;

// Material textures
struct Material
{
    sampler2D albedoTex;
    sampler2D normalTex;
    sampler2D metallicTex;
    sampler2D roughnessTex;
    sampler2D aoTex;
};
uniform Material material;


const float PI = 3.14159265359;

float DistributionGGX(vec3 N, vec3 H, float roughness);
float GeometrySchlickGGX(float NdotV, float roughness);
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness);
vec3 FresnelSchlick(float cosTheta, vec3 F0);
vec3 FresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness);
vec3 CalculateLight(Light light, vec3 V, vec3 N, vec3 F0, vec3 albedo, float metallic, float roughness);
vec3 GetNormalFromMap();

out vec4 FragColor;

void main()
{
    // Get material properties from textures or uniforms
    vec3 albedo = texture(material.albedoTex, TexCoords).rgb;
    float metallicVal = texture(material.metallicTex, TexCoords).r;
    float roughnessVal = texture(material.roughnessTex, TexCoords).r;
    float aoVal = texture(material.aoTex, TexCoords).r;

    // Normal vector
    vec3 N = normalize(Normal);
    vec3 V = normalize(camPos - WorldPos);
    vec3 R = reflect(-V, N); 

    // Calculate reflectance at normal incidence; if dia-electric (like plastic) use F0 
    // of 0.04 and if it's metal, use the albedo color as F0
    vec3 F0 = vec3(0.04);
    F0 = mix(F0, albedo, metallicVal);

    // Reflectance equation
    vec3 Lo = vec3(0.0);
    for(int i = 0; i < lightCount; ++i)
    {
        Lo += CalculateLight(light[i], V, N, F0, albedo, metallicVal, roughnessVal);
    }

    // Ambient lighting (we now use IBL as the ambient term)
    vec3 F = FresnelSchlickRoughness(max(dot(N, V), 0.0), F0, roughnessVal);
    vec3 kD = (1.0 - F) * (1.0 - metallicVal);
    vec3 irradiance = vec3(0.03); // Simple ambient term - replace with IBL in real implementation
    vec3 diffuse = irradiance * albedo;
    vec3 ambient = (kD * diffuse) * aoVal;

    vec3 color = ambient + Lo;

    // HDR tonemapping
    color = color / (color + vec3(1.0));
    // Gamma correct
    color = pow(color, vec3(1.0 / 2.2));

    FragColor = vec4(color, 1.0);
}

// PBR lighting calculation for a single light
vec3 CalculateLight(Light light, vec3 V, vec3 N, vec3 F0, vec3 albedo, float metallic, float roughness)
{
    vec3 L = vec3(0.0);
    vec3 H = vec3(0.0);
    float attenuation = 1.0;

    L = normalize(light.position - WorldPos);
    float distance = length(light.position - WorldPos);
    attenuation = 1.0 / (distance * distance);

    H = normalize(V + L);

    // Radiance
    vec3 radiance = light.color * light.intensity * attenuation;

    // Cook-Torrance BRDF
    float NDF = DistributionGGX(N, H, roughness);
    float G = GeometrySmith(N, V, L, roughness);
    vec3 F = FresnelSchlick(max(dot(H, V), 0.0), F0);

    vec3 numerator = NDF * G * F;
    float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001;
    vec3 specular = numerator / denominator;

    // kS is equal to Fresnel
    vec3 kS = F;
    // For energy conservation, the diffuse and specular light can't
    // be above 1.0 (unless the surface emits light); to preserve this
    // relationship the diffuse component (kD) should equal 1.0 - kS.
    vec3 kD = vec3(1.0) - kS;
    // Multiply kD by the inverse metalness such that only non-metals 
    // have diffuse lighting, or a linear blend if partly metal (pure metals
    // have no diffuse light).
    kD *= 1.0 - metallic;

    // Scale light by NdotL
    float NdotL = max(dot(N, L), 0.0);

    // Add to outgoing radiance Lo
    return (kD * albedo / PI + specular) * radiance * NdotL;
}

// Normal Distribution function
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

// Geometry function
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

// Fresnel function
vec3 FresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

// Fresnel function with roughness
vec3 FresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness)
{
    return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

// Get normal from normal map
vec3 GetNormalFromMap()
{
    vec3 tangentNormal = texture(material.normalTex, TexCoords).xyz * 2.0 - 1.0;
    return normalize(TBN * tangentNormal);
}