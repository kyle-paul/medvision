#version 330 core

layout(location = 0) in vec3 VertPositions;
layout(location = 1) in vec3 vertNormals;
layout(location = 2) in vec2 VertTexCoords;

out vec2 texCoords;
out vec3 fragmentNormals;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
	gl_Position = projection * view * model * vec4(VertPositions, 1.0);
	texCoords = VertTexCoords;
	fragmentNormals = vertNormals;
}