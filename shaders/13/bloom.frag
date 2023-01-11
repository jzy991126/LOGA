#version 330 core
layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightColor;

in VS_OUT {
    vec2 tex_coords;
    vec3 normal;
    vec3 frag_pos;
} fs_in;

struct Light {
    vec3 color;
    vec3 position;
};

uniform sampler2D diff_tex;
uniform Light lights[4];


void main()
{

    vec3 diff_color = texture(diff_tex, fs_in.tex_coords).xyz;
    vec3 normal = normalize(fs_in.normal);
    vec3 lighting = vec3(0.0);
    vec3 ambient = 0.0 * diff_color;
    for (int i = 0; i < 4; i++)
    {
        // diffuse
        vec3 lightDir = normalize(lights[i].position - fs_in.frag_pos);
        float diff = max(dot(lightDir, normal), 0.0);
        vec3 diffuse = lights[i].color * diff * diff_color;
        vec3 result = diffuse;
        // attenuation (use quadratic as we have gamma correction)
        float distance = length(fs_in.frag_pos - lights[i].position);
        result *= 1.0 / (distance * distance);
        lighting += result;
    }
    FragColor = vec4(ambient + lighting, 1.0);
    float brightness = dot(FragColor.rgb, vec3(0.2126, 0.7152, 0.0722));
    if (brightness > 1.0)
    BrightColor = vec4(FragColor.rgb, 1.0);
    else
    BrightColor = vec4(0, 0, 0, 1.0);

}