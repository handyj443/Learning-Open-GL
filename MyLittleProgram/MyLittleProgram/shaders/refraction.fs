#version 330 core
out vec4 FragColor;

in vec3 wPosition;
in vec3 wNormal;

uniform vec3 wEyePosition;
uniform samplerCube skybox;

void main()
{          
	float ratio = 1.00 / 1.52;   
	vec3 wViewVector = wPosition - wEyePosition;
	vec3 refract = refract(wViewVector, wNormal, ratio);
	FragColor = vec4(texture(skybox, refract).rgb, 1.0);
}