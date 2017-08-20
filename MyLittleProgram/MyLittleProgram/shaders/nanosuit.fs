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

//______________________________________________________________________________
// MAIN
void main()
{
	vec3 vNorm = normalize(vNormal);
	vec3 vViewDir = normalize(-vFragPos);

	vec3 result = vec3(0.0, 0.0, 0.0);

	// FragColor = vec4(result, 1.0);
	FragColor = vec4(vNorm, 1.0);
}