#version 330 core

in vec3 vNormal;
in vec3 vFragPos;

out vec4 FragColor;

uniform vec3 objectColor;
uniform vec3 lightColor;
uniform vec3 vLightPos;

void main()
{
	vec3 vNormalN = normalize(vNormal);
	vec3 vLightDirN = normalize(vLightPos - vFragPos);
	float diff = max(dot(vNormalN, vLightDirN), 0.0);
	vec3 diffuse = diff * lightColor;
	
	float specularStrength = 0.5;
	vec3 vViewDirN = normalize(-vFragPos);
	vec3 reflectDirN = reflect(-vLightDirN, vNormalN);
	float spec = pow(max(dot(vViewDirN, reflectDirN), 0.0), 32);
	vec3 specular = specularStrength * spec * lightColor;
	
	vec3 result = (diffuse + specular) * objectColor;
	
	FragColor = vec4(result, 1.0);
	
}