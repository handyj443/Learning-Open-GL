#version 330 core

layout (location = 0) in vec3 mPos;
layout (location = 1) in vec3 mNormal;
layout (location = 2) in vec2 aTexCoords;

out vec3 vNormal;
out vec3 vFragPos;
out vec2 texCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
	gl_Position = projection * view * model * vec4(mPos, 1.0); // clip space
	vFragPos = vec3(view * model * vec4(mPos, 1.0)); // view space
	vNormal = mat3(transpose(inverse(view * model))) * mNormal; // #HACK expensive
	texCoords = aTexCoords;
}