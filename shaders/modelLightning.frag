#version 460

in vec2 TexCoords;
in vec3 Normal;
in vec3 ViewPos;
in vec3 FragPos;
in mat3 TBN;

out vec4 FragColor;

struct Light {
    vec3 position;
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    vec3 color;
    float constant;
    float linear;
    float quadratic;
    float cutOff;
    float outerCutOff;
    float intensity;
    int type;
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
uniform sampler2DArray shininessTexArray;

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
    int shininessTexLayer;
    int hasDiffuse;
    int hasSpecular;
    int hasNormal;
    int hasHeight;
    int hasEmissive;
    int hasAmbientOcclusion;
    int hasMetalness;
    int hasRoughness;
    int hasOpacity;
    int hasShininess;
};

in flat int drawID;
layout (std140, binding = 0) uniform MaterialBuffer {
    GpuMaterial materials[128];
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
    vec3 lightDir;
    if (light[0].type == 2)
    lightDir = normalize(-light[0].direction);
    else
    lightDir = normalize(light[0].position - FragPos);
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

vec3 processPointLight(int index, vec3 materialDiffuse, vec3 materialSpecular, float shininess, float ao, vec3 normal, vec3 viewDir)
{
    float distance = length(light[index].position - FragPos);
    float attenuation = 1.0 / (light[index].constant + light[index].linear * distance + light[index].quadratic * (distance * distance));

    vec3 ambient = light[index].ambient * materialDiffuse * ao;

    vec3 lightDir = normalize(light[index].position - FragPos);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = light[index].diffuse * light[index].color * diff * materialDiffuse;

    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
    vec3 specular = light[index].specular * spec * materialSpecular * light[index].color;

    return (ambient + diffuse + specular) * attenuation;
}

vec3 processDirectionalLight(int index, vec3 materialDiffuse, vec3 materialSpecular, float shininess, float ao, vec3 normal, vec3 viewDir)
{
    vec3 ambient = light[index].ambient * materialDiffuse * ao;

    vec3 lightDir = normalize(-light[index].direction);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = light[index].diffuse * light[index].color * diff * materialDiffuse;


    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
    vec3 specular = light[index].specular * spec * materialSpecular * light[index].color;

    return ambient + diffuse + specular;
}

vec3 processSpotLight(int index, vec3 materialDiffuse, vec3 materialSpecular, float shininess, float ao, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(light[index].position - FragPos);
    float theta = dot(lightDir, normalize(-light[index].direction));
    float epsilon = light[index].cutOff - light[index].outerCutOff;

    if (light[index].intensity > 0.0)
    {
        float distance = length(light[index].position - FragPos);
        float attenuation = 1.0 / (light[index].constant + light[index].linear * distance + light[index].quadratic * (distance * distance));

        vec3 ambient = light[index].ambient * materialDiffuse * ao * attenuation;

        float diff = max(dot(normal, lightDir), 0.0);
        vec3 diffuse = light[index].diffuse * light[index].color * diff * materialDiffuse;


        vec3 reflectDir = reflect(-lightDir, normal);
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
        vec3 specular = light[index].specular * spec * materialSpecular * light[index].color;

        return ambient + (diffuse + specular) * light[index].intensity * attenuation;
    }

    return vec3(0.0);
}

void main()
{
    GpuMaterial mat = materials[drawID];

    vec2 pTexCoords = TexCoords;
    vec3 viewDirTangent = transpose(TBN) * normalize(ViewPos - FragPos);


    if (mat.hasHeight == 1 && mat.hasNormal == 1)
    {
        float height = texture(heightTexArray, vec3(TexCoords, mat.heightTexLayer)).r;
        pTexCoords = calculateParallaxCoords(viewDirTangent, height);
    }

    vec3 materialDiffuse;
    vec3 materialSpecular;


    if (mat.hasDiffuse == 1)
    materialDiffuse = texture(diffuseTexArray, vec3(pTexCoords, mat.diffuseTexLayer)).rgb;
    else
    materialDiffuse = mat.fallbackColor.rgb;



    if (mat.hasSpecular == 1)
    materialSpecular = texture(specularTexArray, vec3(pTexCoords, mat.specularTexLayer)).rgb;
    else
    materialSpecular = vec3(0.2);


    float shininess;

    if (mat.hasShininess == 1)
    shininess = texture(shininessTexArray, vec3(pTexCoords, mat.shininessTexLayer)).r;
    else
    shininess = 32.0;


    float ao = 0.5;
    if (mat.hasAmbientOcclusion == 1)
    ao = texture(ambientOcclusionTexArray, vec3(pTexCoords, mat.ambientOcclusionTexLayer)).r;


    vec3 normal = normalize(Normal);
    if (mat.hasNormal == 1)
    {
        vec3 tangentNormal = texture(normalTexArray, vec3(pTexCoords, mat.normalTexLayer)).xyz * 2.0 - 1.0;


        if (length(tangentNormal) > 0.1)
        normal = normalize(TBN * tangentNormal);

    }


    vec3 emissive = vec3(0.0);
    if (mat.hasEmissive == 1)
    emissive = texture(emissiveTexArray, vec3(pTexCoords, mat.emissiveTexLayer)).rgb;


    vec3 viewDir = normalize(ViewPos - FragPos);
    vec3 totalLighting = vec3(0.0);

    float shadow = (useShadows == 1) ? calculateShadow(FragPosLightSpace) : 0.0;

    for (int i = 0; i < lightCount; i++)
    {
        vec3 lightContribution = vec3(0.0);

        if (light[i].type == 1)
        lightContribution = processPointLight(i, materialDiffuse, materialSpecular, shininess, ao, normal, viewDir);
        else if (light[i].type == 2)
        lightContribution = processDirectionalLight(i, materialDiffuse, materialSpecular, shininess, ao, normal, viewDir);
        else if (light[i].type == 3)
        lightContribution = processSpotLight(i, materialDiffuse, materialSpecular, shininess, ao, normal, viewDir);


        if (i == 0)
        {

            vec3 ambientPart = light[i].ambient * materialDiffuse * ao;
            if (light[i].type == 1 || light[i].type == 3)
            {
                float distance = length(light[i].position - FragPos);
                float attenuation = 1.0 / (light[i].constant + light[i].linear * distance + light[i].quadratic * (distance * distance));
                ambientPart *= attenuation;
            }

            vec3 shadedPart = lightContribution - ambientPart;
            lightContribution = ambientPart + shadedPart * (1.0 - shadow);
        }

        totalLighting += lightContribution;
    }

    vec3 finalColor = totalLighting + emissive;

    float opacity = 1.0;
    if (mat.hasOpacity == 1)
    opacity = texture(opacityTexArray, vec3(pTexCoords, mat.opacityTexLayer)).r;


    if (any(isnan(finalColor)) || any(isinf(finalColor)))
    finalColor = mat.fallbackColor.rgb;

    FragColor = vec4(finalColor, opacity);
}