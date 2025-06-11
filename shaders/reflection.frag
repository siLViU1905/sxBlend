#version 460

out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D reflectionTexture;

void main()
{
    vec4 reflection = texture(reflectionTexture, TexCoords);

    FragColor = mix(reflection, vec4(0.5,0.5,0.5,1.0), 0.2);
}