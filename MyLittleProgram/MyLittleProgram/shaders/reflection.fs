#version 330 core
out vec4 FragColor;

in vec3 wPosition;
in vec3 wNormal;

uniform vec3 wEyePosition;
uniform samplerCube skybox;

void main()
{             
	vec3 wViewVector = wPosition - wEyePosition;
	vec3 reflect = reflect(wViewVector, wNormal);
	FragColor = vec4(texture(skybox, reflect).rgb, 1.0);
}