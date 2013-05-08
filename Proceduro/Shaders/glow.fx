////////////////////////////////////////////////////////////////////////////////
// Filename: glow.fx - make it glow! Gets passed two textures - regular texture
// and a glow texture and then adds them together
////////////////////////////////////////////////////////////////////////////////


/////////////
// GLOBALS //
/////////////
Texture2D regularTexture;
Texture2D glowTexture;
float	glowStrength; //how much the glow will get amplified by

///////////////////
// SAMPLE STATES //
///////////////////
SamplerState SampleType
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Clamp;
    AddressV = Clamp;
};


//////////////
// TYPEDEFS //
//////////////
struct VertexInputType
{
    float4 position : POSITION;
    float2 tex : TEXCOORD0;
};

struct PixelInputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
};

////////////////////////////////////////////////////////////////////////////////
// Vertex Shader
////////////////////////////////////////////////////////////////////////////////
PixelInputType GlowVertexShader(VertexInputType input)
{
    PixelInputType output;
    float texelSize;

	// Change the position vector to be 4 units for proper matrix calculations.
    input.position.w = 1.0f;

    output.position = input.position;
    
	// Store the texture coordinates for the pixel shader.
    output.tex = input.tex;

	return output;
}

////////////////////////////////////////////////////////////////////////////////
// Pixel Shader
////////////////////////////////////////////////////////////////////////////////
float4 GlowPixelShader(PixelInputType input) : SV_Target
{
	float4 textureColor;
    float4 glowColor;
    float4 color;

	//Sample the regular scene texture and the glow map texture.

    // Sample the pixel color from the texture using the sampler at this texture coordinate location.
    textureColor = regularTexture.Sample(SampleType, input.tex);

    // Sample the glow texture.
    glowColor = glowTexture.Sample(SampleType, input.tex);

	/*Add the two textures together to create the final glow. The glow map texture is also multiplied by the glow 
	strength to increase the appearance of the glow effect.*/

    // Add the texture color to the glow color multiplied by the glow stength.
    color = saturate(textureColor + (glowColor * glowStrength));

    return color;
}

////////////////////////////////////////////////////////////////////////////////
// Technique
////////////////////////////////////////////////////////////////////////////////
technique10 GlowTechnique
{
	pass pass0
    {
        SetVertexShader(CompileShader(vs_4_0, GlowVertexShader()));
        SetPixelShader(CompileShader(ps_4_0, GlowPixelShader()));
        SetGeometryShader(NULL);
    }
	
}