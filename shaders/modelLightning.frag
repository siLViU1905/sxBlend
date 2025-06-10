#version 460

in vec2 TexCoords;
in vec3 Normal;
in vec3 ViewPos;
in vec3 FragPos;
in mat3 TBN;

out vec4 FragColor;

struct Light
{
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

vec3 calculateDiffuse(vec3 normal, vec3 lightDir, vec3 lightDiffuse, vec3 materialDiffuse)
{
    float diff = max(dot(normal, lightDir), 0.0);
    return diff * lightDiffuse * materialDiffuse;
}

vec3 calculateSpecular(vec3 normal, vec3 lightDir, vec3 viewDir, vec3 lightSpecular, vec3 materialSpecular, float shininess)
{
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
    return spec * lightSpecular * materialSpecular;
}

float calculateAttenuation(float distance, float constant, float linear, float quadratic)
{
    return 1.0 / (constant + linear * distance + quadratic * (distance * distance));
}

vec2 calculateParallaxCoords(vec3 viewDir, vec4 heightMap)
{
    float height = heightMap.r;

    vec2 p = viewDir.xy / viewDir.z *height;

    return TexCoords - p;
}

in vec4 FragPosLightSpace;
uniform sampler2D shadowMap;


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
struct Material
{
    sampler2D diffuseTex;
    sampler2D specularTex;
    sampler2D normalTex;
    sampler2D emissiveTex;
    sampler2D heightTex;
    float shininess;
};

uniform Material material;

uniform int useShadows;

uniform int hasNormal;
uniform int hasHeightTex;

void main()
{

    vec3 viewDir = normalize(ViewPos - FragPos);

    vec2 pTexCoords = TexCoords;

    if(hasHeightTex == 1)
    {
        vec3 viewDirTangentSpace = transpose(TBN) * viewDir;
        pTexCoords = calculateParallaxCoords(viewDirTangentSpace, texture(material.heightTex, TexCoords));
    }


    vec3 materialDiffuse = texture(material.diffuseTex, pTexCoords).rgb;
    vec3 materialSpecular = texture(material.specularTex, pTexCoords).rgb;

    vec3 ambient = light[0].ambient * materialDiffuse;
    vec3 lighting = ambient;

    float shadow = 0.0;

    if(useShadows == 1)
      shadow = calculateShadow(FragPosLightSpace);

    vec3 normal;

    if(hasNormal == 0)
        normal = normalize(Normal);
    else
    {
        vec3 tangentNormal = texture(material.normalTex, TexCoords).xyz * 2.0 - 1.0;
        tangentNormal = normalize(tangentNormal);

        normal = normalize(TBN * tangentNormal);
    }


    for(int i = 0; i < lightCount; i++)
    {
        vec3 lightDir = normalize(light[i].position - FragPos);

        float distance = length(light[i].position - FragPos);

        float attenuation = calculateAttenuation(distance, light[i].constant, light[i].linear, light[i].quadratic);

        vec3 diffuse = calculateDiffuse(normal, lightDir, light[i].diffuse, materialDiffuse);

        vec3 specular = calculateSpecular(normal, lightDir, viewDir, light[i].specular, materialSpecular, material.shininess);

        vec3 lightContribution = (diffuse + specular) * light[i].color;
        lightContribution *= attenuation;

        lighting += lightContribution;
    }

    lighting *= (1.0 - shadow);

    lighting += texture(material.emissiveTex, TexCoords).rgb;

    FragColor = vec4(lighting, 1.0);
}