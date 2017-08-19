#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D screenTexture;
uniform float screenWidth;
uniform float screenHeight;

void main()
{
	float hOffset = 1.0 / screenWidth;
	float vOffset = 1.0 / screenHeight;
	
    vec2 offsets[9] = vec2[](
        vec2(-hOffset,  vOffset), // top-left
        vec2( 0.0f,    vOffset), // top-center
        vec2( hOffset,  vOffset), // top-right
        vec2(-hOffset,  0.0f),   // center-left
        vec2( 0.0f,    0.0f),   // center-center
        vec2( hOffset,  0.0f),   // center-right
        vec2(-hOffset, -vOffset), // bottom-left
        vec2( 0.0f,   -vOffset), // bottom-center
        vec2( hOffset, -vOffset)  // bottom-right    
    );

    float kernel[9] = float[](
        1, 1,  1,
        1, -8, 1,
        1, 1,  1
    );
    
    vec3 sampleTex[9];
    for(int i = 0; i < 9; i++)
    {
        sampleTex[i] = vec3(texture(screenTexture, TexCoords.st + offsets[i]));
    }
    vec3 col = vec3(0.0);
    for(int i = 0; i < 9; i++)
        col += sampleTex[i] * kernel[i];
    
    FragColor = vec4(col, 1.0);
} 