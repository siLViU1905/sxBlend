#version 460

out vec4 FragColor;

in vec3 WorldPos;
in vec2 TexCoords;

uniform float maxHeight;

uniform vec3 grassColor;
uniform vec3 rockColor;
uniform vec3 snowColor;

uniform int hasTex;

uniform sampler2D grassTexture;
uniform sampler2D rockTexture;
uniform sampler2D snowTexture;

void main()
{
  float height = WorldPos.y / maxHeight;

  vec3 grass, rock, snow;

  if(hasTex == 0)
  {
    grass = grassColor;
    rock = rockColor;
    snow = snowColor;
  }
  else
  {
    grass = texture(grassTexture, TexCoords).rgb;
    rock = texture(rockTexture, TexCoords).rgb;
    snow = texture(snowTexture, TexCoords).rgb;
  }

  vec3 finalColor = mix(grass, rock, smoothstep(0.3, 0.5, height));
  finalColor = mix(finalColor, snow, smoothstep(0.7, 0.9, height));

  FragColor = vec4(finalColor, 1.0);
}