#version 430 core
layout (location = 0) in vec3 aPos;
uniform mat4 inv_view;
uniform mat4 inv_proj;



out vec3 nearPoint;
out vec3 farPoint;

vec3 UnprojectPoint(vec3 p) {
    vec4 unprojectedPoint = inv_view * inv_proj * vec4(p, 1.0);
    return unprojectedPoint.xyz / unprojectedPoint.w;
}

void main()
{

    nearPoint = UnprojectPoint(vec3(aPos.x, aPos.y, -1.0));
    farPoint = UnprojectPoint(vec3(aPos.x, aPos.y, 1.0));
    gl_Position = vec4(aPos, 1.0);
}