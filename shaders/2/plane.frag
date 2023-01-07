#version 430 core
out vec4 FragColor;

in vec3 nearPoint;
in vec3 farPoint;
in vec3 world_pos;

uniform mat4 view;
uniform mat4 projection;

#define far 100
#define near 0.1

vec4 grid(vec3 fragPos3D, float scale) {

    vec2 ddx = dFdx(fragPos3D.xz);
    vec2 ddy = dFdy(fragPos3D.xz);

    vec2 w = max(abs(ddx), abs(ddy)) + 0.01;
    vec2 p = fragPos3D.xz;
    vec2 a = p + 0.5 * w;
    vec2 b = p - 0.5 * w;
    vec2 i = (floor(a) + min(fract(a) * scale, 1.0) - floor(b) - min(fract(b) * scale, 1.0)) / (scale * w);
    float s = max((1.0 - i.x), (1.0 - i.y));

    vec2 coord = fragPos3D.xz * scale; // use the scale variable to set the distance between the lines
    vec2 derivative = fwidth(coord);
    vec2 grid = abs(fract(coord - 0.5) - 0.5) / derivative;
    float line = min(grid.x, grid.y);
    float minimumz = min(derivative.y, 1);
    float minimumx = min(derivative.x, 1);
    vec4 color = vec4(0.2, 0.2, 0.2, 1.0 - line);
    // z axis
    if (fragPos3D.x > -0.1 * minimumx && fragPos3D.x < 0.1 * minimumx)
    color.z = 1.0;
    // x axis
    if (fragPos3D.z > -0.1 * minimumz && fragPos3D.z < 0.1 * minimumz)
    color.x = 1.0;
    return color;
}

float computeDepth(vec3 pos) {
    vec4 clip_space_pos = projection * view * vec4(pos.xyz, 1.0);
    return (clip_space_pos.z / clip_space_pos.w);
}
float computeLinearDepth(vec3 pos) {
    vec4 clip_space_pos = projection * view * vec4(pos.xyz, 1.0);
    float clip_space_depth = (clip_space_pos.z / clip_space_pos.w) * 2.0 - 1.0; // put back between -1 and 1
    float linearDepth = (2.0 * near * far) / (far + near - clip_space_depth * (far - near)); // get linear value between 0.01 and 100
    return linearDepth / far; // normalize
}
void main()
{
    //    float t = - nearPoint.y / (farPoint.y - nearPoint.y);
    //    float ok = float(nearPoint.y < 0 && farPoint.y > 0);
    //    vec3 fragPos3D = nearPoint + t * (farPoint - nearPoint);
    //    gl_FragDepth = computeDepth(fragPos3D);
    //    FragColor = vec4(1.0, 0.0, 0.0, 1.0 * float(t > 0)); // opacity = 1 when t > 0, opacity = 0 otherwise
    //    //FragColor = grid(fragPos3D, 10) * float(t > 0);
    //
    float linearDepth = computeLinearDepth(world_pos);
    float fading = max(0, (0.5 - linearDepth));

    FragColor = (grid(world_pos, 10) + grid(world_pos, 1)); // adding multiple resolution for the grid
    FragColor.a *= fading;

    //FragColor = grid(world_pos, 10);




}