#version 330

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexcoords;
layout (location = 3) in vec3 tangent;
layout (location = 4) in vec3 bitangent;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform vec3 light_pos;
uniform vec3 view_pos;

out VS_OUT {
    vec2 texcoords;
    vec3 t_position;
    vec3 t_light_pos;
    vec3 t_view_pos;
} vs_out;

void main() {

    //    vs_out.texcoords = aTexcoords;
    //    mat4 normal_mat = transpose(inverse(model));
    //
    //    vec3 T = normalize(vec3(model * vec4(tangent, 0.0)));
    //    vec3 B = normalize(vec3(model * vec4(bitangent, 0.0)));
    //    vec3 N = normalize(vec3(model * vec4(aNormal, 0.0)));
    //    mat3 TBN = transpose(mat3(T, B, N));
    //
    //    vs_out.t_position = TBN * vec3(model * vec4(aPos, 1.0));
    //    vs_out.t_light_pos = TBN * light_pos;
    //    vs_out.t_view_pos = TBN * view_pos;
    //
    //    gl_Position = projection * view * model * vec4(aPos, 1.0);

    gl_Position = projection * view * model * vec4(aPos, 1.0f);
    vs_out.texcoords = aTexcoords;


    vec3 T = normalize(mat3(model) * tangent);
    vec3 B = normalize(mat3(model) * bitangent);
    vec3 N = normalize(mat3(model) * aNormal);
    mat3 TBN = transpose(mat3(T, B, N));

    vs_out.t_light_pos = TBN * light_pos;
    vs_out.t_view_pos = TBN * view_pos;
    vs_out.t_position = TBN * vec3(model * vec4(aPos, 1.0));

}