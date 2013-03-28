//=============================================================================
// watermagic.fx
//
//=============================================================================

#include "lighthelper.fx"

cbuffer cbPerFrame{
	Light	gLight;
	int		gLightType;
	float3	gEyePosW;
};

cbuffer cbPerObject{
	float4x4	worldMatrix;
	float4x4	viewMatrix;
	float4x4	projectionMatrix;	
};

//////////////
// TYPEDEFS //
//////////////
struct VertexInputType{
    float3 position : POSITION;
	float3 normal   : NORMAL;
};

struct PixelInputType{
    float4 positionH	: SV_POSITION;
	float3 positionW	: POSITION;
	float4 normal		: NORMAL;
};

////////////////////////////////////////////////////////////////////////////////
// Vertex Shader
////////////////////////////////////////////////////////////////////////////////
PixelInputType WaterVertexShader(VertexInputType input){
    PixelInputType output;       

	// Perform wave equation interpolation

    // Calculate the position of the vertex against the world, view, and projection matrices.
    output.positionW = mul(float4(input.position, 1.0f), worldMatrix);
	output.normal	= mul(float4(input.normal, 0.0f), worldMatrix);

	// Store the texture coordinates for the pixel shader.
	output.positionH = mul(float4(input.position, 1.0f), worldMatrix);
    output.positionH = mul(output.positionH, viewMatrix);
    output.positionH = mul(output.positionH, projectionMatrix);
    
    return output;
}

////////////////////////////////////////////////////////////////////////////////
// Pixel Shader
////////////////////////////////////////////////////////////////////////////////
float4 WaterPixelShader(PixelInputType input) : SV_Target
{
	// Interpolating normal can make it not be of unit length so normalize it.
    float3 normalW = normalize(input.normal);

	float4 spec = (0,0,0,0.7f);//spec.a is the alpha

	// Get materials from texture maps.
	float4 diffuse = float4(0.2f,0.65f,0.98f,1.0f);	
    
	// Compute the lit color for this pixel.
	SurfaceInfo v = {input.positionW, normalW, diffuse, spec};
	float3 litColor;
	if( gLightType == 0 ) // Parallel
    {
		litColor = ParallelLight(v, gLight, gEyePosW); 
    }
    else if( gLightType == 1 ) // Point
    {
		litColor = PointLight(v, gLight, gEyePosW);
	}
	else // Spot
	{
		litColor = Spotlight(v, gLight, gEyePosW);
	}

	return float4(litColor, spec.a);	
}

////////////////////////////////////////////////////////////////////////////////
// Technique
////////////////////////////////////////////////////////////////////////////////
technique10 WaterTechnique
{
    pass pass0
    {
        SetVertexShader(CompileShader(vs_4_0, WaterVertexShader()));
		SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_4_0, WaterPixelShader()));
        
    }
}