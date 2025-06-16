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
uniform vec3 aColor;

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

out vec4 FragColor;

vec3 processLight(int index, vec3 materialColor, vec3 normal)
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

uniform int hasTex;

uniform int useShadows;

uniform sampler2D _texture;

void main()
{
    vec3 result = vec3(0.0);

    float shadow = 0.0;

    if(useShadows == 1)
     shadow = calculateShadow(FragPosLightSpace);

    vec3 normal = normalize(Normal);

    if(hasTex == 0)
        for(int i = 0; i < lightCount; i++) 
        result += processLight(i, aColor, normal);
    else
        for(int i = 0; i < lightCount; i++) 
        result += processLight(i, texture(_texture, TexCoords).rgb, normal);

    result *= (1.0 - shadow);
  
    FragColor = vec4(result, 1.0);
}