#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;

uniform float ti;

out vec3 ourColor;

void main()
{
    gl_Position = vec4(aPos.x+ti,aPos.y+ti,aPos.z, 1.0);
    ourColor = aPos;
}