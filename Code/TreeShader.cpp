////////////////////////////////////////////////////////////////////////////////
// Filename: TreeShader.cpp
////////////////////////////////////////////////////////////////////////////////
#include "TreeShader.h"


TreeShader::TreeShader()
{
	mWorldMatrix		= 0;
	mViewMatrix			= 0;
	mProjectionMatrix	= 0;
}


TreeShader::TreeShader(const TreeShader& other)
{
}


TreeShader::~TreeShader()
{
	mWorldMatrix		= 0;
	mViewMatrix			= 0;
	mProjectionMatrix	= 0;
}


bool TreeShader::Initialize(ID3D10Device* device, HWND hwnd)
{
	bool result;


	// Initialize the shader that will be used to draw the model.
	result = InitializeShader(device, hwnd, L"Shaders/treemagic.fx");
	if(!result)
	{
		return false;
	}

	return true;
}


void TreeShader::Render(ID3D10Device* device, int indexCount, D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix, D3DXMATRIX projectionMatrix, 
				D3DXVECTOR3 mEyePos, Light lightVar, int lightType)
{
	// Set the shader parameters that it will use for rendering.
	SetShaderParameters(worldMatrix, viewMatrix, projectionMatrix, mEyePos, lightVar, lightType);

	// Now render the prepared buffers with the shader.
	RenderShader(device, indexCount);

	return;
}


bool TreeShader::InitializeShader(ID3D10Device* device, HWND hwnd, WCHAR* filename)
{
	HRESULT result;
	ID3D10Blob* errorMessage;
	
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
	mTechnique = mEffect->GetTechniqueByName("TreeTechnique");
	if(!mTechnique)
	{
		return false;
	}

	D3D10_INPUT_ELEMENT_DESC polygonLayout[3];

	// Now setup the layout of the data that goes into the shader.
	// This setup needs to match the VertexType stucture in the ModelClass and in the shader.
	polygonLayout[0].SemanticName = "POSITION";
	polygonLayout[0].SemanticIndex = 0;
	polygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[0].InputSlot = 0;
	polygonLayout[0].AlignedByteOffset = 0;
	polygonLayout[0].InputSlotClass = D3D10_INPUT_PER_VERTEX_DATA;
	polygonLayout[0].InstanceDataStepRate = 0;
	
	polygonLayout[1].SemanticName = "PARENTPOSITION";
	polygonLayout[1].SemanticIndex = 0;
	polygonLayout[1].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[1].InputSlot = 0;
	polygonLayout[1].AlignedByteOffset = D3D10_APPEND_ALIGNED_ELEMENT;
	polygonLayout[1].InputSlotClass = D3D10_INPUT_PER_VERTEX_DATA;
	polygonLayout[1].InstanceDataStepRate = 0;

	polygonLayout[2].SemanticName = "RADIUS";
	polygonLayout[2].SemanticIndex = 0;
	polygonLayout[2].Format = DXGI_FORMAT_R32G32_FLOAT;
	polygonLayout[2].InputSlot = 0;
	polygonLayout[2].AlignedByteOffset = D3D10_APPEND_ALIGNED_ELEMENT;
	polygonLayout[2].InputSlotClass = D3D10_INPUT_PER_VERTEX_DATA;
	polygonLayout[2].InstanceDataStepRate = 0;

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
	mWorldMatrix =	mEffect->GetVariableByName("worldMatrix")->AsMatrix();
	mViewMatrix =	mEffect->GetVariableByName("viewMatrix")->AsMatrix();
	mProjectionMatrix = mEffect->GetVariableByName("projectionMatrix")->AsMatrix();

	mEyePosVar		= mEffect->GetVariableByName("gEyePosW");

	mLightVar		= mEffect->GetVariableByName("gLight");
	mLightType		= mEffect->GetVariableByName("gLightType")->AsScalar();

	return true;
}


void TreeShader::SetShaderParameters(D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix, D3DXMATRIX projectionMatrix, 
				D3DXVECTOR3 mEyePos, Light lightVar, int lightType)
{
	LightShader::SetShaderParameters(worldMatrix,viewMatrix,projectionMatrix,mEyePos,lightVar,lightType);

	// Set the eye position variable inside the shader
	mEyePosVar->SetRawValue(&mEyePos, 0, sizeof(D3DXVECTOR3));

	// Set the light variable inside the shader
	mLightVar->SetRawValue(&lightVar, 0, sizeof(Light));

	// Set the light type inside the shader
	mLightType->SetInt(lightType);

	return;
}