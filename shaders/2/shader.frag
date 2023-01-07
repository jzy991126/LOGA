#version 330 core

struct Material {
    sampler2D diffuse;
    sampler2D specular;
    sampler2D emission;
    float shininess;
};


struct Light {
    vec3 position;
    //vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;
};




out vec4 FragColor;

uniform vec3 objectColor;
uniform vec3 lightColor;
uniform vec3 view_pos;
uniform Light light;
uniform Material material;

uniform float emission_strength;
uniform float matrix_move;

in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoords;

void main()
{
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(light.position - FragPos);
    vec3 diffuse_color = texture(material.diffuse, TexCoords).xyz;


    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * light.diffuse * diffuse_color;


    float ambient_strngth = 0.1f;
    vec3 ambient = diffuse_color * light.ambient;


    vec3 view_dir = normalize(view_pos - FragPos);
    vec3 reflect_dir = reflect(-lightDir, norm);
    float sepc = pow(max(dot(view_dir, reflect_dir), 0.0), 32);
    vec3 spec_color = texture(material.specular, TexCoords).xyz;
    vec3 specualar = spec_color * sepc * light.specular;

    vec3 emission = texture(material.emission, vec2(TexCoords.x, TexCoords.y + matrix_move)).xyz * emission_strength;

    float dist = length(light.position - FragPos);
    float attenuation = 1.0 / (light.constant + light.linear * dist + light.quadratic * (dist * dist));

    vec3 res = (specualar + ambient + diffuse) * attenuation;


    FragColor = vec4(res, 1.0);
}