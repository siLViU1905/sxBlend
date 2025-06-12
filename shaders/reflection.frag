#version 460

out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D reflectionTexture;

uniform vec3 aColor;

void main()
{
    vec4 reflection = texture(reflectionTexture, TexCoords);

    FragColor = mix(reflection, vec4(aColor,1.0), 0.5);
}