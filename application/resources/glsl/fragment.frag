#version 330 core

in vec2 texCoords;
in vec3 fragmentNormals;
layout(location = 0) out vec4 texRender;

uniform sampler2D ImageTexture;
const vec3 sunDirection = normalize(vec3(-300.0, 300.0, 300.0));

void main()
{
	float lightStrength = max(0.5, dot(fragmentNormals, sunDirection));
	texRender = vec4(1.5 * lightStrength * texture(ImageTexture, texCoords).rgb, 1.0);
}