#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D screenTexture;

void main()
{
    FragColor = vec4(texture(screenTexture, TexCoords).xxx, 1.0);
}