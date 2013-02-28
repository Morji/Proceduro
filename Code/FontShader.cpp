////////////////////////////////////////////////////////////////////////////////
// Filename: FontShader.cpp
////////////////////////////////////////////////////////////////////////////////
#include "FontShader.h"


FontShader::FontShader()
{
	mWorldMatrix		= 0;
	mViewMatrix			= 0;
	mProjectionMatrix	= 0;
	mTexture = 0;
	mPixelColor = 0;
}


FontShader::FontShader(const FontShader& other)
{
}


FontShader::~FontShader()
{
	mWorldMatrix		= 0;
	mViewMatrix			= 0;
	mProjectionMatrix	= 0;
	mTexture = 0;
	mPixelColor = 0;
}


bool FontShader::Initialize(ID3D10Device* device, HWND hwnd)
{
	bool result;


	// Initialize the shader that will be used to draw the triangle.
	result = InitializeShader(device, hwnd, L"Shaders/font.fx");
	if(!result)
	{
		return false;
	}

	return true;
}

void FontShader::Render(ID3D10Device* device, int indexCount, D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix,
							 D3DXMATRIX projectionMatrix, ID3D10ShaderResourceView* texture, D3DXVECTOR4 pixelColor)
{
	// Set the shader parameters that it will use for rendering.
	SetShaderParameters(worldMatrix, viewMatrix, projectionMatrix, texture, pixelColor);

	// Now render the prepared buffers with the shader.
	RenderShader(device, indexCount);

	return;
}


bool FontShader::InitializeShader(ID3D10Device* device, HWND hwnd, WCHAR* filename)
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
		// If there was  nothing in the error message then it simply could not find the shader file itself.
		else
		{
			MessageBox(hwnd, filename, L"Missing Shader File", MB_OK);
		}

		return false;
	}

	// Get a pointer to the technique inside the shader.
	mTechnique = mEffect->GetTechniqueByName("FontTechnique");
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
    result = device->CreateInputLayout(polygonLayout, numElements, passDesc.pIAInputSignature, passDesc.IAInputSignatureSize, 
									   &mLayout);
	if(FAILED(result))
	{
		return false;
	}
		
	// Get pointers to the three matrices inside the shader so we can update them from this class.
    mWorldMatrix = mEffect->GetVariableByName("worldMatrix")->AsMatrix();
	mViewMatrix = mEffect->GetVariableByName("viewMatrix")->AsMatrix();
    mProjectionMatrix = mEffect->GetVariableByName("projectionMatrix")->AsMatrix();

	// Get pointer to the texture resource inside the shader.
	mTexture = mEffect->GetVariableByName("shaderTexture")->AsShaderResource();

	// Get pointer to the pixel color variable inside the shader.
	mPixelColor = mEffect->GetVariableByName("pixelColor")->AsVector();

	return true;
}


void FontShader::SetShaderParameters(D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix, D3DXMATRIX projectionMatrix,
										  ID3D10ShaderResourceView* texture, D3DXVECTOR4 pixelColor)
{
	// Set the world matrix variable inside the shader.
    mWorldMatrix->SetMatrix((float*)&worldMatrix);

	// Set the view matrix variable inside the shader.
	mViewMatrix->SetMatrix((float*)&viewMatrix);

	// Set the projection matrix variable inside the shader.
    mProjectionMatrix->SetMatrix((float*)&projectionMatrix);

	// Bind the texture.
	mTexture->SetResource(texture);

	// Set the pixel color variable inside the shader with the pixelColor vector.
	mPixelColor->SetFloatVector((float*)&pixelColor);

	return;
}