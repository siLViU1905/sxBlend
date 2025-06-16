#version 460 core

in vec2 TexCoords;
in vec3 Normal;
in vec3 ViewPos;
in vec3 FragPos;
in mat3 TBN;
in vec3 WorldPos;

in vec4 FragPosLightSpace;
uniform sampler2D shadowMap;
uniform int useShadows;

struct Light
{
    vec3 position;
    vec3 color;
    int type;
};

uniform Light light[6];
uniform int lightCount;

uniform vec3 aColor;
uniform float metallic;
uniform float roughness;
uniform float ao;

const float PI = 3.14159265359;

float distributionGGX(vec3 N, vec3 H, float roughness)
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

float geometrySchlickGGX(float NdotV, float roughness)
{
    float r = roughness + 1.0;
    float k = (r * r) / 8.0;

    float nom = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return nom / denom;
}

float geometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = geometrySchlickGGX(NdotV, roughness);
    float ggx1 = geometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}

vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

out vec4 FragColor;

uniform int hasTex;

uniform sampler2D _texture;


float calculateShadow(vec4 fpls)
{
    vec3 projCoords = fpls.xyz / fpls.w;

    projCoords = projCoords * 0.5 + 0.5;

    if(projCoords.x < 0.0 || projCoords.x > 1.0 ||
    projCoords.y < 0.0 || projCoords.y > 1.0 ||
    projCoords.z < 0.0 || projCoords.z > 1.0)
    return 0.0;

    float currentDepth = projCoords.z;

    vec3 lightDir = normalize(light[0].position - FragPos);
    float bias = max(0.05 * (1.0 - dot(Normal, lightDir)), 0.005);

    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    for(int x = -3; x <= 3; ++x)
    {
        for(int y = -3; y <= 3; ++y)
        {
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
            shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
        }
    }
    shadow /= 49.0;

    return shadow;
}

void main()
{
    vec3 N = normalize(Normal);
    vec3 V = normalize(ViewPos - WorldPos);

    vec3 usedColor;

    if(hasTex == 0)
        usedColor = aColor;
    else
        usedColor = texture(_texture, TexCoords).rgb;

    vec3 F0 = vec3(0.04);
    F0 = mix(F0, usedColor, metallic);

    vec3 Lo = vec3(0.0);

    float shadow = (useShadows == 1) ? calculateShadow(FragPosLightSpace) : 0.0;

    for(int i = 0; i < lightCount; i++)
    {
        vec3 L = normalize(light[i].position - WorldPos);
        vec3 H = normalize(V + L);
        float distance = length(light[i].position - WorldPos);
        float attenuation = 1.0 / (distance * distance);
        vec3 radiance = light[i].color * attenuation;

        float NDF = distributionGGX(N, H, roughness);
        float G = geometrySmith(N, V, L, roughness);
        vec3 F = fresnelSchlick(clamp(dot(H, V), 0.0, 1.0), F0);

        vec3 numerator = NDF * G * F;
        float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001;
        vec3 specular = numerator / denominator;

        vec3 kS = F;

        vec3 kD = vec3(1.0) - kS;

        kD *= 1.0 - metallic;
        float NdotL = max(dot(N, L), 0.0);
        vec3 lightContribution = (kD * usedColor / PI + specular) * radiance * NdotL;

        if(i==0)
         lightContribution *= (1.0 - shadow);

        Lo += lightContribution;

    }

    vec3 ambient = vec3(0.03) * usedColor * ao;

    vec3 color = ambient + Lo;

    color = color / (color + vec3(1.0));

    color = pow(color, vec3(1.0 / 2.2));

    FragColor = vec4(color, 1.0);
}