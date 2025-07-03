#version 460

in vec2 TexCoords;
in vec3 Normal;
in vec3 ViewPos;
in vec3 FragPos;
in mat3 TBN;

in vec4 FragPosLightSpace;
uniform sampler2D shadowMap;

struct Light
{
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
uniform vec3 aColor;
uniform int hasTex;
uniform int useShadows;
uniform sampler2D _texture;

out vec4 FragColor;

float calculateShadow(vec4 fpls)
{
    vec3 projCoords = fpls.xyz / fpls.w;
    projCoords = projCoords * 0.5 + 0.5;

    if(projCoords.x < 0.0 || projCoords.x > 1.0 ||
        projCoords.y < 0.0 || projCoords.y > 1.0 ||
        projCoords.z < 0.0 || projCoords.z > 1.0)
        return 0.0;

    float currentDepth = projCoords.z;

    vec3 lightDir;
    if(light[0].type == 2)
        lightDir = normalize(-light[0].direction);
    else
        lightDir = normalize(light[0].position - FragPos);

    float bias = max(0.05 * (1.0 - dot(Normal, lightDir)), 0.005);

    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);

    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
            shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
        }
    }
    shadow /= 9.0;

    return shadow;
}

vec3 processPointLight(int index, vec3 materialColor, vec3 normal)
{
    float distance = length(light[index].position - FragPos);
    float attenuation = 1.0 / (light[index].constant +
        light[index].linear * distance +
        light[index].quadratic * (distance * distance));

    vec3 ambient = light[index].ambient * materialColor;

    vec3 lightDir = normalize(light[index].position - FragPos);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = light[index].diffuse * light[index].color * diff * materialColor;

    vec3 viewDir = normalize(ViewPos - FragPos);
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), 32.0);
    vec3 specular = light[index].specular * spec * light[index].color;

    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;

    return ambient + diffuse + specular;
}

vec3 processDirectionalLight(int index, vec3 materialColor, vec3 normal)
{
    vec3 ambient = light[index].ambient * materialColor;

    vec3 lightDir = normalize(-light[index].direction);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = light[index].diffuse * light[index].color * diff * materialColor;

    vec3 viewDir = normalize(ViewPos - FragPos);
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), 32.0);
    vec3 specular = light[index].specular * spec * light[index].color;

    return ambient + diffuse + specular;
}

vec3 processSpotLight(int index, vec3 materialColor, vec3 normal)
{
    vec3 lightDir = normalize(light[index].position - FragPos);
    float theta = dot(lightDir, normalize(-light[index].direction));

    float distance = length(light[index].position - FragPos);
    float attenuation = 1.0 / (light[index].constant +
        light[index].linear * distance +
        light[index].quadratic * (distance * distance));

    float epsilon = light[index].cutOff - light[index].outerCutOff;

    float spotIntensity = clamp((theta - light[index].outerCutOff) / epsilon, 0.0, 1.0);

    spotIntensity = smoothstep(0.0, 1.0, spotIntensity);

    vec3 ambient = light[index].ambient * materialColor;

    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = light[index].diffuse * light[index].color * diff * materialColor;

    vec3 viewDir = normalize(ViewPos - FragPos);
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), 32.0);
    vec3 specular = light[index].specular * spec * light[index].color;

    ambient *= attenuation;
    diffuse *= attenuation * light[index].intensity * spotIntensity;
    specular *= attenuation * light[index].intensity * spotIntensity;

    return ambient + diffuse + specular;

}

void main()
{
    vec3 result = vec3(0.0);
    vec3 normal = normalize(Normal);

    vec3 materialColor = (hasTex == 1) ? texture(_texture, TexCoords).rgb : aColor;

    float shadow = (useShadows == 1) ? calculateShadow(FragPosLightSpace) : 0.0;

    for(int i = 0; i < lightCount; i++)
    {
        vec3 lightContribution = vec3(0.0);

        if(light[i].type == 1)
            lightContribution = processPointLight(i, materialColor, normal);
        else if(light[i].type == 2)
            lightContribution = processDirectionalLight(i, materialColor, normal);
        else if(light[i].type == 3)
            lightContribution = processSpotLight(i, materialColor, normal);

        if(i == 0)
            lightContribution *= (1.0 - shadow);

        result += lightContribution;
    }

    FragColor = vec4(result, 1.0);
}