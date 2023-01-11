#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;


out VS_OUT {
    vec2 tex_coords;
    vec3 normal;
    vec3 frag_pos;
} vs_out;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    vs_out.tex_coords = aTexCoords;
    mat3 normalMatrix = mat3(transpose(inverse(model)));
    vs_out.normal = normalize(vec3(vec4(normalMatrix * (-aNormal), 0.0)));
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    vs_out.frag_pos = vec3(model * vec4(aPos, 1.0));
}