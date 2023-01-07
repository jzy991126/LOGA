#version 330 core

struct Material {
    sampler2D diffuse;
    sampler2D specular;
    sampler2D emission;
    float shininess;
};


struct Light {
    vec3 position;
    vec3 direction;

    float cutOff;
    float outerCutOff;

    vec3 ambient;
    sampler2D diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;
};

struct DirLight {
    vec3 direction;

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
uniform mat4 view;


in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoords;

uniform DirLight dirLight;

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(- light.direction);
    // 漫反射着色
    float diff = max(dot(normal, lightDir), 0.0);
    // 镜面光着色
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // 合并结果
    vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoords));
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoords));
    vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoords));
    return (ambient + diffuse + specular);
}

struct PointLight {
    vec3 position;

    float constant;
    float linear;
    float quadratic;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
#define NR_POINT_LIGHTS 4
uniform PointLight pointLights[NR_POINT_LIGHTS];

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);
    // 漫反射着色
    float diff = max(dot(normal, lightDir), 0.0);
    // 镜面光着色
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // 衰减
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance +
    light.quadratic * (distance * distance));
    // 合并结果
    vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoords));
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoords));
    vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoords));
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
    return (ambient + diffuse + specular);
}




void main()
{
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(light.position - FragPos);
    vec3 diffuse_color = texture(material.diffuse, TexCoords).xyz;
    float theta = dot(lightDir, normalize(- light.direction));

    //    vec2 uv = gl_FragCoord.xy / reslotion;
    //    uv = (uv + 1.) / 2.;
    vec4 view_light_dir4 = view * vec4(lightDir, 0.0);
    vec3 view_light_dir = normalize(view_light_dir4.xyz);
    vec2 uv = vec2(0.5, 0.5);
    float rad = acos(light.outerCutOff);
    vec2 zz = normalize(vec2(view_light_dir.x, view_light_dir.y));
    float view_alpha = acos(abs(view_light_dir.z));
    float ratio = tan(view_alpha) / tan(rad);
    vec2 zzz = zz * (ratio / 2.f);
    uv = uv + zzz;

    float diff = max(dot(norm, lightDir), 0.0);
    vec3 light_diffuse = texture(light.diffuse, uv).xyz;

    vec3 diffuse = diff * light_diffuse * diffuse_color;



    vec3 ambient = diffuse_color * light.ambient;


    vec3 view_dir = normalize(view_pos - FragPos);
    vec3 reflect_dir = reflect(-lightDir, norm);
    float sepc = pow(max(dot(view_dir, reflect_dir), 0.0), 32);
    vec3 spec_color = texture(material.specular, TexCoords).xyz;
    vec3 specualar = spec_color * sepc * light.specular;

    //    vec3 emission = texture(material.emission, vec2(TexCoords.x, TexCoords.y + matrix_move)).xyz * emission_strength;

    float dist = length(light.position - FragPos);
    float attenuation = 1.0 / (light.constant + light.linear * dist + light.quadratic * (dist * dist));


    float episilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / episilon, 0.0, 1.0);


    vec3 res = (specualar * intensity + ambient + diffuse * intensity);

    vec3 res1 = CalcDirLight(dirLight, norm, view_dir);
    vec3 res2 = vec3(0, 0, 0);
    for (int i = 0;i < NR_POINT_LIGHTS; i++)
        res2 += CalcPointLight(pointLights[0], norm, FragPos, view_dir);
    //vec3 res = light_diffuse * intensity;
    FragColor = vec4(res + res1 + res2, 1.0);

    //    if (theta > light.cutOff)
    //    FragColor = vec4(res, 1.0);
    //    else
    //    FragColor = vec4(ambient, 1.0);
}