#version 330 core
layout(location = 0) in vec3 inPosition;

uniform mat4 DirectionPv;

void main()
{
    gl_Position = DirectionPv * vec4(inPosition, 1.0);
}