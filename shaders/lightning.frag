#version 460

in vec2 TexCoords;
in vec3 Normal;
in vec3 ViewPos;
in vec3 FragPos;
in mat3 TBN;

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

out vec4 FragColor;

vec3 processLight(int index, vec3 materialColor, vec3 normal)
{
    float distance = length(light[index].position - FragPos);
    float attenuation = 1.0 / (light[index].constant +
        light[index].linear * distance +
        light[index].quadratic * (distance * distance));

    // Ambient
    vec3 ambient = light[index].ambient * materialColor;

    // Diffuse
    vec3 lightDir = normalize(light[index].position - FragPos);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = light[index].diffuse * light[index].color * diff * materialColor;

    // Specular (Blinn-Phong)
    vec3 viewDir = normalize(ViewPos - FragPos);
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), 32.0);
    vec3 specular = light[index].specular * spec * light[index].color;

    // Aplicarea atenuării
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;

    return ambient + diffuse + specular;
}

uniform int hasTex;

uniform sampler2D _texture;

void main()
{
    vec3 result = vec3(0.0);

    vec3 normal = normalize(Normal);

    if(hasTex == 0)
        for(int i = 0; i < lightCount; i++) 
        result += processLight(i, aColor, normal);
    else
        for(int i = 0; i < lightCount; i++) 
        result += processLight(i, texture(_texture, TexCoords).rgb, normal);

    FragColor = vec4(result, 1.0);
}