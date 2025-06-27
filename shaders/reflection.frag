#version 460

out vec4 FragColor;

in vec2 TexCoords;
in vec3 FragPos;
in vec3 ViewPos;
in vec3 Normal;
in vec2 reflectionTexCoords;

uniform sampler2D reflectionTexture;

uniform int useSkybox;
uniform samplerCube skybox;

uniform float metallic;
uniform float roughness;
uniform float ao;

uniform vec3 aColor;

uniform int useFlatReflection;

struct Reflection
{
  vec3 reflection;
    float fresnel;
};

Reflection computeFlatReflection(vec3 N, vec3 V, float roughness)
{
    Reflection result;
    vec3 R = reflect(-V, N);

    vec4 planarRefl = texture(reflectionTexture, reflectionTexCoords);
    vec3 envRefl = useSkybox == 1 ? textureLod(skybox, R, roughness * 4.0).rgb : vec3(0.0);

    result.fresnel = pow(1.0 - max(dot(N, V), 0.0), 5.0);
    result.fresnel = mix(0.04, 1.0, result.fresnel);

    float reflStrength = 1.0 - roughness * 0.8;

    result.reflection = mix(planarRefl.rgb, envRefl, result.fresnel) * reflStrength;

    return result;
}

Reflection computeCurvedReflection(vec3 N, vec3 V, float roughness)
{
    Reflection result;
    vec3 R = reflect(-V, N);
    
    vec4 planarRefl = texture(reflectionTexture, reflectionTexCoords);
    vec3 envRefl = useSkybox == 1 ? textureLod(skybox, R, roughness * 4.0).rgb : vec3(0.0);
    
    result.fresnel = pow(1.0 - max(dot(N, V), 0.04), 5.0);
    float planarWeight = smoothstep(0.2, 0.8, roughness);
    result.reflection = mix(envRefl, planarRefl.rgb, planarWeight * (1.0 - result.fresnel));
    
    return result;
}

void main()
{
    vec3 N = normalize(Normal);
    vec3 V = normalize(ViewPos - FragPos);

    Reflection refl;

    if (useFlatReflection == 1)
    refl = computeFlatReflection(N, V, roughness);
    else
    refl = computeCurvedReflection(N, V, roughness);

    vec3 diffuse = aColor * (1.0 - metallic);
    vec3 specular = mix(vec3(0.04), aColor, metallic);

    vec3 color = (diffuse * ao) + (refl.reflection * specular * refl.fresnel);

    FragColor = vec4(color, 1.0);
}