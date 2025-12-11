#version 330 core

layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec3 inNormal;
layout (location = 2) in vec2 inTextureCoordinate;
layout (location = 3) in vec3 inTangent;
layout (location = 4) in vec3 inBitangent;

uniform mat4 view;
uniform mat4 projection;
uniform mat4 model;

uniform vec3 lightPosition;
uniform vec3 viewPosition;

out VS_OUT {
    vec3 fragmentPosition;
    vec2 textureCoordinate;
    vec3 TangentLightPosition;
    vec3 TangentViewPosition;
    vec3 TangentFragmentPosition;
} vs_out;

void main()
{
    gl_Position = projection * view * model * vec4(inPosition, 1.0);

    vs_out.textureCoordinate = inTextureCoordinate;
    vs_out.fragmentPosition = vec3(model * vec4(inPosition, 1.0));
    
    mat3 normalMatrix = transpose(inverse(mat3(model)));
    vec3 T = normalize(normalMatrix * inTangent);
    vec3 N = normalize(normalMatrix * inNormal);
    // vec3 B = normalize(normalMatrix * inBitangent);
    T = normalize(T - dot(T, N) * N);
    vec3 B = cross(N, T);

    mat3 TBN = transpose(mat3(T, B, N));
    vs_out.TangentLightPosition = TBN * lightPosition;
    vs_out.TangentViewPosition = TBN * viewPosition;
    vs_out.TangentFragmentPosition = TBN * vs_out.fragmentPosition;
}