#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;

out vec3 ourColor;
uniform mat3 uTransform;

void main()
{
    vec3 worldPos = uTransform * aPos;
    gl_Position = vec4(aPos, 1.0);
    ourColor = aColor;
}
