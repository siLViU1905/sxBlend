#version 460

out vec4 FragColor;

in vec2 TexCoords;
in vec3 FragPos;
in vec3 ViewPos;
in vec3 Normal;

uniform sampler2D reflectionTexture;

uniform int useSkybox;
uniform samplerCube skybox;

uniform float metallic;
uniform float roughness;
uniform float ao;

uniform vec3 aColor;

void main()
{
   vec3 N = normalize(Normal);
   vec3 V = normalize(ViewPos - FragPos);

   vec3 R = reflect(-V, N);

   vec3 finalRefl;
   vec3 envRefl = vec3(0.0);
   float mipLevel = roughness * 8.0;

   if(useSkybox == 1)
      envRefl = textureLod(skybox, R, mipLevel).rgb;

   vec4 planarRefl = texture(reflectionTexture, TexCoords);

   float fresnel = pow(1.0 - max(dot(N, V), 0.0), 5.0);
   fresnel = mix(0.04, 1.0, fresnel);

   float reflStrength = 1.0 - roughness * 0.8;

   finalRefl = mix(planarRefl.rgb, envRefl, fresnel) * reflStrength;

   vec3 diffuse = aColor * (1.0 - metallic);
   vec3 specular = mix(vec3(0.04), aColor, metallic);

   vec3 color = (diffuse * ao) + (finalRefl * specular * fresnel);

   FragColor = vec4(color, 1.0);
}