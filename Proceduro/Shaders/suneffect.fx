////////////////////////////////////////////////////////////////////////////////
// Filename: suneffect.fx
////////////////////////////////////////////////////////////////////////////////


/////////////
// GLOBALS //
/////////////
cbuffer cbPerObject
{
    matrix worldMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
};

cbuffer cbPerFrame
{
    float4 regularColor;
    float4 glowColor;
};

//////////////
// TYPEDEFS //
//////////////
struct VertexInputType
{
    float4 position : POSITION;
};

struct PixelInputType
{
    float4 position : SV_POSITION;
};


////////////////////////////////////////////////////////////////////////////////
// Vertex Shader
////////////////////////////////////////////////////////////////////////////////
PixelInputType SunVertexShader(VertexInputType input)
{
    PixelInputType output;
    
    // Change the position vector to be 4 units for proper matrix calculations.
    input.position.w = 1.0f;

    // Calculate the position of the vertex against the world, view, and projection matrices.
    output.position = mul(input.position, worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);

    return output;
}


////////////////////////////////////////////////////////////////////////////////
// Pixel Shader
////////////////////////////////////////////////////////////////////////////////
float4 SunPixelShader(PixelInputType input) : SV_Target
{
    float glowStrength = 0.8f;

    return saturate(regularColor + (glowColor * glowStrength));
}


////////////////////////////////////////////////////////////////////////////////
// Technique
////////////////////////////////////////////////////////////////////////////////
technique10 SunTechnique
{
    pass pass0
    {
        SetVertexShader(CompileShader(vs_4_0, SunVertexShader()));
        SetPixelShader(CompileShader(ps_4_0, SunPixelShader()));
        SetGeometryShader(NULL);
    }
}