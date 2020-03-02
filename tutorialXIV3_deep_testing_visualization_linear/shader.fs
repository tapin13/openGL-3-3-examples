#version 330 core

float near = 0.1;
float far = 100.0;

in vec2 textureCoordinates;

uniform sampler2D texture1;

out vec4 outColor;

float LinearizeDepth(float depth) {
    float z = depth * 2.0 - 1.0;
    return (2.0 * near * far) / (far + near - z * (far - near));
}

void main()
{
    // outColor = texture(texture1, textureCoordinates);
    // outColor = vec4(vec3(gl_FragCoord.z), 1.0);
    float depth = LinearizeDepth(gl_FragCoord.z) / far;
    outColor = vec4(vec3(depth), 1.0);
}
