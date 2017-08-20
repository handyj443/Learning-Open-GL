#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

out vec3 wPosition;
out vec3 wNormal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
	wPosition = vec3(model * vec4(aPos, 1.0));
    wNormal = mat3(transpose(inverse(model))) * aNormal; // #HACK expensive;
    gl_Position = projection * view * model * vec4(aPos, 1.0);
}