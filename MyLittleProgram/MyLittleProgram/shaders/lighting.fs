#version 330 core

in vec3 wNormal;
in vec3 wFragPos;

out vec4 FragColor;

uniform vec3 objectColor;
uniform vec3 lightColor;
uniform vec3 wLightPos;
uniform vec3 wViewPos;

void main()
{
	vec3 wNormalN = normalize(wNormal);
	vec3 lightDirN = normalize(wLightPos - wFragPos);
	float diff = max(dot(wNormalN, lightDirN), 0.0);
	vec3 diffuse = diff * lightColor;
	
	float specularStrength = 0.5;
	vec3 viewDirN = normalize(wViewPos - wFragPos);
	vec3 reflectDirN = reflect(-lightDirN, wNormalN);
	float spec = pow(max(dot(viewDirN, reflectDirN), 0.0), 32);
	vec3 specular = specularStrength * spec * lightColor;
	
	vec3 result = (diffuse + specular) * objectColor;
	
	FragColor = vec4(result, 1.0);
	
}