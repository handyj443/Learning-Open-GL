#version 330 core

in vec3 vNormal;
in vec3 vFragPos;

out vec4 FragColor;

struct Material {
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float shininess;
};
uniform Material material;

struct Light {
	vec3 position;
	
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};
uniform Light light;

void main()
{
	vec3 ambient = light.ambient * material.ambient;
	
	vec3 vNormalN = normalize(vNormal);
	vec3 vLightDirN = normalize(light.position - vFragPos);
	float diff = max(dot(vNormalN, vLightDirN), 0.0);
	vec3 diffuse = light.diffuse * diff * material.diffuse;
	
	vec3 vViewDirN = normalize(-vFragPos);
	vec3 reflectDirN = reflect(-vLightDirN, vNormalN);
	float spec = pow(max(dot(vViewDirN, reflectDirN), 0.0), material.shininess);
	vec3 specular = light.specular * spec * material.specular;
	
	vec3 result = ambient + diffuse + specular;
	
	FragColor = vec4(result, 1.0);
}