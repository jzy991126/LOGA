#version 430 core
layout (location = 0) in vec3 aPos;
uniform mat4 inv_view;
uniform mat4 inv_proj;
uniform mat4 view;
uniform mat4 projection;



out vec3 nearPoint;
out vec3 farPoint;
out vec3 world_pos;

vec3 UnprojectPoint(vec3 p) {
    vec4 unprojectedPoint = inv_view * inv_proj * vec4(p, 1.0);
    return unprojectedPoint.xyz / unprojectedPoint.w;
}

void main()
{

    //    nearPoint = UnprojectPoint(vec3(aPos.x, aPos.y,-1));
    //    farPoint = UnprojectPoint(vec3(aPos.x, aPos.y, 1));
    //    gl_Position = vec4(aPos, 1.0);
    vec3 pos = aPos * 100;
    world_pos = pos;
    gl_Position = projection * view * vec4(pos, 1.0);
}