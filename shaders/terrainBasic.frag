#version 460

out vec4 FragColor;

in vec3 WorldPos;

uniform float maxHeight;

uniform vec3 grassColor;
uniform vec3 rockColor;
uniform vec3 snowColor;

void main()
{
  float height = WorldPos.y / maxHeight;

  vec3 finalColor = mix(grassColor, rockColor, smoothstep(0.3,0.5,height));
  finalColor = mix(finalColor, snowColor,smoothstep(0.7,0.9,height));

  FragColor = vec4(finalColor, 1.0);
}