#include "GlowShader.h"


GlowShader::GlowShader(void)
{
	mRegularTexture = nullptr;
	mGlowTexture = nullptr;
	mGlowStrength = nullptr;
}

GlowShader::~GlowShader(void)
{
	mRegularTexture = nullptr;
	mGlowTexture = nullptr;
	mGlowStrength = nullptr;
}

bool GlowShader::Initialize(ID3D10Device* device, HWND hwnd)
{
	bool result;

	// Initialize the shader that will be used to draw the triangle.
	result = InitializeShader(device, hwnd, L"Shaders/glow.fx");
	if(!result){
		return false;
	}

	return true;
}

void GlowShader::Render(ID3D10Device* device, int indexCount, ID3D10ShaderResourceView* regularTexture, ID3D10ShaderResourceView* glowTexture, float glowStrength)
{
	// Set the shader parameters that it will use for rendering.
	SetShaderParameters(regularTexture, glowTexture, glowStrength);

	// Now render the prepared buffers with the shader.
	RenderShader(device, indexCount);
}

void GlowShader::SetShaderParameters(ID3D10ShaderResourceView* regularTexture, ID3D10ShaderResourceView* glowTexture, float glowStrength)
{
	mRegularTexture->SetResource(regularTexture);
	mGlowTexture->SetResource(glowTexture);
	mGlowStrength->SetFloat(glowStrength);
}

bool GlowShader::InitializeShader(ID3D10Device* device, HWND hwnd, WCHAR* filename)
{
	HRESULT result;
	ID3D10Blob* errorMessage;
	D3D10_INPUT_ELEMENT_DESC polygonLayout[2];
	unsigned int numElements;
    D3D10_PASS_DESC passDesc;

	// Initialize the error message.
	errorMessage = 0;

	// Load the shader in from the file.
	result = D3DX10CreateEffectFromFile(filename, NULL, NULL, "fx_4_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, 
										device, NULL, NULL, &mEffect, &errorMessage, NULL);
	if(FAILED(result))
	{
		// If the shader failed to compile it should have writen something to the error message.
		if(errorMessage)
		{
			OutputShaderErrorMessage(errorMessage, hwnd, filename);
		}
		// If there was nothing in the error message then it simply could not find the shader file itself.
		else
		{
			MessageBox(hwnd, filename, L"Missing Shader File", MB_OK);
		}

		return false;
	}

	// Get a pointer to the technique inside the shader.
	mTechnique = mEffect->GetTechniqueByName("GlowTechnique");
	if(!mTechnique)
	{
		return false;
	}

	// Now setup the layout of the data that goes into the shader.
	// This setup needs to match the VertexType stucture in the ModelClass and in the shader.
	polygonLayout[0].SemanticName = "POSITION";
	polygonLayout[0].SemanticIndex = 0;
	polygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[0].InputSlot = 0;
	polygonLayout[0].AlignedByteOffset = 0;
	polygonLayout[0].InputSlotClass = D3D10_INPUT_PER_VERTEX_DATA;
	polygonLayout[0].InstanceDataStepRate = 0;

	polygonLayout[1].SemanticName = "TEXCOORD";
	polygonLayout[1].SemanticIndex = 0;
	polygonLayout[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	polygonLayout[1].InputSlot = 0;
	polygonLayout[1].AlignedByteOffset = D3D10_APPEND_ALIGNED_ELEMENT;
	polygonLayout[1].InputSlotClass = D3D10_INPUT_PER_VERTEX_DATA;
	polygonLayout[1].InstanceDataStepRate = 0;

	// Get a count of the elements in the layout.
    numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

	// Get the description of the first pass described in the shader technique.
    mTechnique->GetPassByIndex(0)->GetDesc(&passDesc);

	// Create the input layout.
    result = device->CreateInputLayout(polygonLayout, numElements, passDesc.pIAInputSignature, passDesc.IAInputSignatureSize, &mLayout);
	if(FAILED(result))
	{
		return false;
	}

	// Get pointer to the texture resources inside the shader.
	mRegularTexture = mEffect->GetVariableByName("regularTexture")->AsShaderResource();
	mGlowTexture = mEffect->GetVariableByName("glowTexture")->AsShaderResource();

	// Get a pointer to the glow strength variable inside the shader
	mGlowStrength = mEffect->GetVariableByName("glowStrength")->AsScalar();

	return true;
}