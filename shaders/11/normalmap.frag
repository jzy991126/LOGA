#version 330

uniform sampler2D diff_tex;
uniform sampler2D norm_tex;
uniform sampler2D disp_tex;
uniform float height_scale;
out vec4 FragColor;



uniform bool norm_map;

in VS_OUT {
    vec2 texcoords;
    vec3 t_position;
    vec3 t_light_pos;
    vec3 t_view_pos;
} fs_in;

vec2 ParallaxMapping(vec2 texCoords, vec3 viewDir)
{
    //    float height = texture(disp_tex, texCoords).r;
    //    return texCoords - viewDir.xy / viewDir.z * (height * height_scale);
    // number of depth layers
    const float minLayers = 10;
    const float maxLayers = 20;
    float numLayers = mix(maxLayers, minLayers, abs(dot(vec3(0.0, 0.0, 1.0), viewDir)));
    // calculate the size of each layer
    float layerDepth = 1.0 / numLayers;
    // depth of current layer
    float currentLayerDepth = 0.0;
    // the amount to shift the texture coordinates per layer (from vector P)
    vec2 P = viewDir.xy * height_scale;
    vec2 deltaTexCoords = P / numLayers;


    vec2 currentTexCoords = texCoords;
    float currentDepthMapValue = texture(disp_tex, currentTexCoords).r;

    while (currentLayerDepth < currentDepthMapValue)
    {
        // shift texture coordinates along direction of P
        currentTexCoords -= deltaTexCoords;
        // get depthmap value at current texture coordinates
        currentDepthMapValue = texture(disp_tex, currentTexCoords).r;
        // get depth of next layer
        currentLayerDepth += layerDepth;
    }

    vec2 prevTexCoords = currentTexCoords + deltaTexCoords;

    // get depth after and before collision for linear interpolation
    float afterDepth = currentDepthMapValue - currentLayerDepth;
    float beforeDepth = texture(disp_tex, prevTexCoords).r - currentLayerDepth + layerDepth;

    // interpolation of texture coordinates
    float weight = afterDepth / (afterDepth - beforeDepth);
    vec2 finalTexCoords = prevTexCoords * weight + currentTexCoords * (1.0 - weight);

    return finalTexCoords;
}



void main() {
    vec3 view_dir = normalize(fs_in.t_view_pos - fs_in.t_position);
    vec2 texCoords = fs_in.texcoords;
    vec3 light_dir = normalize(fs_in.t_light_pos - fs_in.t_position);

    texCoords = ParallaxMapping(fs_in.texcoords, view_dir);
    if (texCoords.x > 1.0 || texCoords.y > 1.0 || texCoords.x < 0.0 || texCoords.y < 0.0)
    discard;

    vec3 norm = texture(norm_tex, texCoords).xyz;
    norm = norm * 2.0 - 1.0;
    norm = normalize(norm);

    vec3 diff_color = texture(diff_tex, texCoords).rgb;


    vec3 ambient = 0.1 * diff_color;

    float diff = max(dot(norm, light_dir), 0.0);
    vec3 diffuse = diff * diff_color;

    vec3 reflect_dir = normalize(reflect(-light_dir, norm));
    vec3 half_dir = normalize(light_dir + view_dir);
    float spec = pow(max(dot(norm, half_dir), 0.0), 32.0);
    vec3 specular = spec * vec3(0.2);

    FragColor = vec4(ambient + diffuse + specular, 1.0);

}