#version 330 core

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};


struct Light {
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};




out vec4 FragColor;

uniform vec3 objectColor;
uniform vec3 lightColor;
uniform vec3 view_pos;
uniform Light light;
uniform Material material;

in vec3 Normal;
in vec3 FragPos;

void main()
{
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(light.position - FragPos);


    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * light.diffuse*material.diffuse;


    float ambient_strngth = 0.1f;
    vec3 ambient = material.ambient * light.ambient;

    float specular_strength = 0.5;
    vec3 view_dir = normalize(view_pos - FragPos);
    vec3 reflect_dir = reflect(-lightDir, norm);
    float sepc = pow(max(dot(view_dir, reflect_dir), 0.0), 32);
    vec3 specualar = material.specular * sepc * light.specular;

    vec3 res = (specualar + ambient + diffuse) ;


    FragColor = vec4(res, 1.0);
}