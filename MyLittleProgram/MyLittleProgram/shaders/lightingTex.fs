#version 330 core

in vec3 vNormal;
in vec3 vFragPos;
in vec2 texCoords;

out vec4 FragColor;

struct Material {
	sampler2D diffuse;
	sampler2D specular;
	float shininess;
};
uniform Material material;

struct Light {
	vec3 vPosition;
	
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	
	float constant;
	float linear;
	float quadratic;
};
uniform Light light;

void main()
{
	float distance = length(light.vPosition - vFragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance +
                               light.quadratic * (distance * distance));

    vec3 materialDiffuseSample = vec3(texture(material.diffuse, texCoords));
	vec3 ambient = light.ambient * materialDiffuseSample;
	
	vec3 vNormalN = normalize(vNormal);
	vec3 vLightDirN = normalize(light.vPosition - vFragPos);
	float diff = max(dot(vNormalN, vLightDirN), 0.0);
	vec3 diffuse = light.diffuse * diff * materialDiffuseSample;
	
	vec3 vViewDirN = normalize(-vFragPos);
	vec3 reflectDirN = reflect(-vLightDirN, vNormalN);
	float spec = pow(max(dot(vViewDirN, reflectDirN), 0.0), material.shininess);
	vec3 specular = light.specular * spec * vec3(texture(material.specular, texCoords));
	
	ambient  *= attenuation; 
	diffuse  *= attenuation;
	specular *= attenuation;  
	
	vec3 result = ambient + diffuse + specular;
	
	FragColor = vec4(result, 1.0);
}