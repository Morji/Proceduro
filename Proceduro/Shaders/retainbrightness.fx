////////////////////////////////////////////////////////////////////////////////
// Filename: retainbrightness.fx - intended to output a texture 
// that only retains bright pixels and discards the rest
////////////////////////////////////////////////////////////////////////////////


/////////////
// GLOBALS //
/////////////
Texture2D shaderTexture;
float	luminanceThreshold; //how luminant? a pixel has to be in order to glow (0 is not bright at all, 1 is fully bright)

///////////////////
// SAMPLE STATES //
///////////////////
SamplerState SampleType
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Wrap;
    AddressV = Wrap;
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
PixelInputType RetainBrightnessVertexShader(VertexInputType input)
{
    PixelInputType output;
    float texelSize;

	// Change the position vector to be 4 units for proper matrix calculations.
    input.position.w = 1.0f;

	// Calculate the position of the vertex against the world, view, and projection matrices.
    output.position = input.position;
    
	// Store the texture coordinates for the pixel shader.
    output.tex = input.tex;

	return output;
}

////////////////////////////////////////////////////////////////////////////////
// Pixel Shader
////////////////////////////////////////////////////////////////////////////////
float4 RetainBrightnessPixelShader(PixelInputType input) : SV_Target
{
	float4 color;

	// Initialize the color to black.
	color = shaderTexture.Sample(SampleType, input.tex);

	// calculate luminance of pixel - if below threshold, discard
	// Color brightness calculation formula based on http://stackoverflow.com/questions/596216/formula-to-determine-brightness-of-rgb-color
	float luminance = (0.299f*color.r) + (0.587f *color.g) + (0.114*color.b);
	if (luminance < luminanceThreshold){
		discard;
	}
	color.a = 1.0f;
	return color;    
}

////////////////////////////////////////////////////////////////////////////////
// Technique
////////////////////////////////////////////////////////////////////////////////
technique10 RetainBrightnessTechnique
{
	pass pass0
    {
        SetVertexShader(CompileShader(vs_4_0, RetainBrightnessVertexShader()));
        SetPixelShader(CompileShader(ps_4_0, RetainBrightnessPixelShader()));
        SetGeometryShader(NULL);
    }
	
}