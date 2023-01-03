#version 330 core
out vec4 FragColor;

in vec3 ourColor;
in vec2 texCoord;

uniform sampler2D ourTexture;
uniform sampler2D ourTexture1;

void main()
{
    FragColor = mix(texture(ourTexture,texCoord),texture(ourTexture1, texCoord), 0.2);
    //FragColor = texture(ourTexture1, texCoord);
    //FragColor = texture(ourTexture,texCoord);
}