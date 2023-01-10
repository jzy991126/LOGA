#version 330

uniform sampler2D diff_tex;
uniform sampler2D norm_tex;
out vec4 FragColor;



uniform bool norm_map;

in VS_OUT {
    vec2 texcoords;
    vec3 t_position;
    vec3 t_light_pos;
    vec3 t_view_pos;
} fs_in;

void main() {

    vec3 light_dir = normalize(fs_in.t_light_pos - fs_in.t_position);
    vec3 diff_color = texture(diff_tex, fs_in.texcoords).xyz;
    vec3 norm;

    norm = texture(norm_tex, fs_in.texcoords).xyz;
    norm = norm * 2.0 - 1.0;
    norm = normalize(norm);

    vec3 view_dir = normalize(fs_in.t_view_pos - fs_in.t_position);

    vec3 ambient = 0.1 * diff_color;

    float diff = max(dot(norm, light_dir), 0.0);
    vec3 diffuse = diff * diff_color;

    vec3 reflect_dir = normalize(reflect(-light_dir, norm));
    vec3 half_dir = normalize(light_dir + view_dir);
    float spec = pow(max(dot(norm, half_dir), 0.0), 32.0);
    vec3 specular = spec * vec3(0.2);

    FragColor = vec4(ambient + diffuse + specular, 1.0);
    
}