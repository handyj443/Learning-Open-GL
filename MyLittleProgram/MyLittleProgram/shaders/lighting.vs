#version 330 core

layout (location = 0) in vec3 aPos; // model space
layout (location = 1) in vec3 aNormal; // model space

out vec3 wNormal;
out vec3 wFragPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
	gl_Position = projection * view * model * vec4(aPos, 1.0); // clip space
	wFragPos = vec3(model * vec4(aPos, 1.0)); // world space
	wNormal = mat3(transpose(inverse(model))) * aNormal; // #HACK expensive
}