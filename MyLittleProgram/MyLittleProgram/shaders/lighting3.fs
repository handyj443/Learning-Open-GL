#version 330 core

in vec3 vNormal;
in vec3 vFragPos;
in vec2 texCoords;

out vec4 FragColor;

//______________________________________________________________________________
// UNIFORMS
struct Material
{
	sampler2D texture_diffuse0;
	sampler2D texture_specular0;
	float shininess;
};
uniform Material material;

// LIGHTS
struct DirLight
{
	vec3 vDirection;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};
uniform DirLight dirLight;

struct PointLight
{
	vec3 vPosition;

	float constant;
	float linear;
	float quadratic;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};
#define NR_POINT_LIGHTS 4
uniform PointLight pointLights[NR_POINT_LIGHTS];

// Spotlight
struct SpotLight
{
	vec3 vPosition;
	vec3 vDirection;

	float innerCutOff;
	float outerCutOff;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	float constant;
	float linear;
	float quadratic;
};
uniform SpotLight spotLight;

//______________________________________________________________________________
// FWD DECLARATIONS
vec3 CalcDirLight(DirLight dirLight, vec3 vNormal, vec3 vViewDir);
vec3 CalcPointLight(PointLight pointLight, vec3 vNormal, vec3 vFragPos,
					vec3 vViewDir);
vec3 CalcSpotLight(SpotLight spotLight, vec3 vNormal, vec3 vFragPos,
				   vec3 vViewDir);
//______________________________________________________________________________
// MAIN
void main()
{
	vec3 vNorm = normalize(vNormal);
	vec3 vViewDir = normalize(-vFragPos);

	vec3 result = vec3(0.0, 0.0, 0.0);
	result += CalcDirLight(dirLight, vNorm, vViewDir);
	for (int i = 0; i < 4; i++)
	{
		result += CalcPointLight(pointLights[i], vNorm, vFragPos, vViewDir);
	}
	result += CalcSpotLight(spotLight, vNorm, vFragPos, vViewDir);

	FragColor = vec4(result, 1.0);
//	FragColor = vec4(vNorm, 1.0);
}

vec3 CalcDirLight(DirLight dirLight, vec3 vNormal, vec3 vViewDir)
{
	vec3 vLightDir = normalize(-dirLight.vDirection);
	// diffuse shading
	float diff = max(dot(vNormal, vLightDir), 0.0);
	// specular shading
	vec3 vReflectDir = reflect(-vLightDir, vNormal);
	float spec = pow(max(dot(vViewDir, vReflectDir), 0.0), material.shininess);
	// combine results
	vec3 ambient
		= dirLight.ambient * vec3(texture(material.texture_diffuse0, texCoords));
	vec3 diffuse
		= dirLight.diffuse * diff * vec3(texture(material.texture_diffuse0, texCoords));
	vec3 specular = dirLight.specular * spec
		* vec3(texture(material.texture_specular0, texCoords));
	return (ambient + diffuse + specular);
}

vec3 CalcPointLight(PointLight pointLight, vec3 vNormal, vec3 vFragPos,
					vec3 vViewDir)
{
	vec3 vLightDir = normalize(pointLight.vPosition - vFragPos);
	// diffuse shading
	float diff = max(dot(vNormal, vLightDir), 0.0);
	// specular shading
	vec3 vReflectDir = reflect(-vLightDir, vNormal);
	float spec = pow(max(dot(vViewDir, vReflectDir), 0.0), material.shininess);
	// attenuation
	float distance = length(pointLight.vPosition - vFragPos);
	float attenuation = 1.0
		/ (pointLight.constant + pointLight.linear * distance
		   + pointLight.quadratic * (distance * distance));
	// combine results
	vec3 ambient
		= pointLight.ambient * vec3(texture(material.texture_diffuse0, texCoords));
	vec3 diffuse = pointLight.diffuse * diff
		* vec3(texture(material.texture_diffuse0, texCoords));
	vec3 specular = pointLight.specular * spec
		* vec3(texture(material.texture_specular0, texCoords));
	ambient *= attenuation;
	diffuse *= attenuation;
	specular *= attenuation;
	return (ambient + diffuse + specular);
}

vec3 CalcSpotLight(SpotLight spotLight, vec3 vNormal, vec3 vFragPos,
				   vec3 vViewDir)
{
	vec3 result;
	float distance = length(spotLight.vPosition - vFragPos);
	float attenuation = 1.0
		/ (spotLight.constant + spotLight.linear * distance
		   + spotLight.quadratic * (distance * distance));

	vec3 ambient
		= spotLight.ambient * vec3(texture(material.texture_diffuse0, texCoords));

	vec3 vLightDirN = normalize(spotLight.vPosition - vFragPos);
	float theta = dot(vLightDirN, normalize(-spotLight.vDirection));
	if (theta > spotLight.outerCutOff)
	{
		// Diffuse
		vec3 vNormalN = normalize(vNormal);
		float diff = max(dot(vNormalN, vLightDirN), 0.0);
		vec3 diffuse = spotLight.diffuse * diff * vec3(texture(material.texture_diffuse0, texCoords));

		// Specular
		vec3 vViewDirN = normalize(-vFragPos);
		vec3 reflectDirN = reflect(-vLightDirN, vNormalN);
		float spec
			= pow(max(dot(vViewDirN, reflectDirN), 0.0), material.shininess);
		vec3 specular = spotLight.specular * spec
			* vec3(texture(material.texture_specular0, texCoords));

		// Rim fade
		float epsilon = spotLight.innerCutOff - spotLight.outerCutOff;
		float intensity
			= clamp((theta - spotLight.outerCutOff) / epsilon, 0.0, 1.0);

		ambient *= attenuation;
		diffuse *= attenuation * intensity;
		specular *= attenuation * intensity;

		result = ambient + diffuse + specular;
	}
	else
	{
		result = ambient * attenuation;
	}
	return result;
}