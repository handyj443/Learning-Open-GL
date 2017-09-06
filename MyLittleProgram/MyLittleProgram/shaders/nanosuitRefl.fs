#version 330 core

in vec3 wPosition;
in vec3 wNormal;
in vec2 texCoords;

out vec4 FragColor;

//______________________________________________________________________________
// UNIFORMS
struct Material
{
	sampler2D texture_diffuse0;
	sampler2D texture_specular0;
	sampler2D texture_reflection0;
};
uniform Material material;

uniform vec3 wEyePosition;
uniform samplerCube skybox;


//______________________________________________________________________________
// MAIN
void main()
{
	vec3 result;
	
	vec3 wViewVector = wPosition - wEyePosition;
	vec3 reflect = reflect(wViewVector, wNormal);


	vec3 diffuseCol = texture(material.texture_diffuse0, texCoords).rgb;
	vec3 reflectionMask = texture(material.texture_reflection0, texCoords).rgb;
	vec3 reflectionCol = texture(skybox, reflect).rgb * 2;

	result = reflectionCol * reflectionMask + diffuseCol * (1 - reflectionMask);

	FragColor = vec4(result, 1.0);
}