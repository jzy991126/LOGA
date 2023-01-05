#version 330 core
out vec4 FragColor;

uniform vec3 objectColor;
uniform vec3 lightColor;
uniform vec3 light_pos;
uniform vec3 view_pos;

in vec3 Normal;
in vec3 FragPos;

void main()
{
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(light_pos - FragPos);


    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;


    float ambient_strngth = 0.1f;
    vec3 ambient = ambient_strngth * lightColor;

    float specular_strength = 0.5;
    vec3 view_dir = normalize(view_pos - FragPos);
    vec3 reflect_dir = reflect(-lightDir, norm);
    float sepc = pow(max(dot(view_dir, reflect_dir), 0.0), 32);
    vec3 specualar = specular_strength * sepc * lightColor;

    vec3 res = (specualar + ambient + diffuse) * objectColor;


    FragColor = vec4(res, 1.0);
}