#version 330 core

in vec3 outPosition;
in vec3 outNormal;

uniform vec3 cameraPos;
uniform samplerCube cubemap;

out vec4 outColor;

void main()
{
    vec3 I = normalize(outPosition - cameraPos);
    vec3 R = reflect(I, normalize(outNormal));
    outColor = vec4(texture(cubemap, R).rgb, 1.0);
}
