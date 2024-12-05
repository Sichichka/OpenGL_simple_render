#version 330 core

out vec4 outColor;

uniform vec3 lightColor;
uniform bool wireframeMode;

void main()
{
	if (wireframeMode)
		outColor = vec4(lightColor, 1.0f);
	else
		outColor = vec4(1.0f, 1.0f, 1.0f, 1.0f);
}