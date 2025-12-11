#version 330 core

in vec3 outPosition;
in vec3 outNormal;

uniform vec3 cameraPos;
uniform samplerCube cubemap;

out vec4 outColor;

void main()
{
    float refraction_ratio = 1.0 / 1.52; // 1.52 - glass
    // float refraction_ratio = 1.0 / 2.42; // 2.42 - Diamond
    vec3 I = normalize(outPosition - cameraPos);
    vec3 R = refract(I, normalize(outNormal), refraction_ratio);
    outColor = vec4(texture(cubemap, R).rgb, 1.0);
}
