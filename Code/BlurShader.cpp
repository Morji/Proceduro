////////////////////////////////////////////////////////////////////////////////
// Filename: BlurShader.cpp
////////////////////////////////////////////////////////////////////////////////
#include "BlurShader.h"


BlurShader::BlurShader()
{
	mTexture = nullptr;
	mScreenHeight = nullptr;
	for (int i = 0; i < 5; i++){
		mWeights[i] = nullptr;
	}
}


BlurShader::BlurShader(const BlurShader& other)
{
}


BlurShader::~BlurShader()
{
	mTexture = nullptr;
	mScreenHeight = nullptr;
	for (int i = 0; i < 5; i++){
		mWeights[i] = nullptr;
	}
}


bool BlurShader::Initialize(ID3D10Device* device, HWND hwnd)
{
	bool result;


	// Initialize the shader that will be used to draw the model.
	result = InitializeShader(device, hwnd, L"Shaders/blur.fx");
	if(!result)
	{
		return false;
	}

	return true;
}

void BlurShader::SetWeights(float weigths[5]){
	float normalization = weigths[0];
	for (int i = 1; i < 5; i++){
		normalization += 2*weigths[i];
	}
	for (int i = 0; i < 5; i++){
		mWeights[i]->SetFloat(weigths[i]/normalization);
	}
}

void BlurShader::RenderHorizontalBlur(ID3D10Device* device, int indexCount,  ID3D10ShaderResourceView* texture, float width)
{
	// Set the shader parameters that it will use for rendering.
	// Bind the texture.
	mTexture->SetResource(texture);
	// Set the screen width inside the shader.		
	mScreenWidth->SetFloat(width);
	
	// Set the input layout.
	device->IASetInputLayout(mLayout);
	
	mTechnique->GetPassByIndex(0)->Apply(0);
	device->DrawIndexed(indexCount, 0, 0);
	return;
}


void BlurShader::RenderVerticalBlur(ID3D10Device* device, int indexCount, ID3D10ShaderResourceView* texture, float height){
	// Set the shader parameters that it will use for rendering.
	// Bind the texture.
	mTexture->SetResource(texture);
	// Set the screen height inside the shader.			
	mScreenHeight->SetFloat(height);	

	// Set the input layout.
	device->IASetInputLayout(mLayout);

	mTechnique->GetPassByIndex(1)->Apply(0);
	device->DrawIndexed(indexCount, 0, 0);
	return;
}

bool BlurShader::InitializeShader(ID3D10Device* device, HWND hwnd, WCHAR* filename)
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
	mTechnique = mEffect->GetTechniqueByName("VerticalBlurTechnique");
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

	// Get the description of the second pass described in the shader technique.
    mTechnique->GetPassByIndex(1)->GetDesc(&passDesc);

	// Create the input layout.
	result = device->CreateInputLayout(polygonLayout, numElements, passDesc.pIAInputSignature, passDesc.IAInputSignatureSize, &mLayoutVertical);
	if(FAILED(result))
	{
		return false;
	}

	// Get pointer to the texture resource inside the shader.
	mTexture = mEffect->GetVariableByName("shaderTexture")->AsShaderResource();

	// Get a pointer to the screen height inside the shader.
	mScreenHeight = mEffect->GetVariableByName("screenHeight")->AsScalar();

	// Get a pointer to the screen width inside the shader.
	mScreenWidth = mEffect->GetVariableByName("screenWidth")->AsScalar();

	// Get a pointer to all weights inside the shader
	mWeights[0] = mEffect->GetVariableByName("weight0")->AsScalar();
	mWeights[1] = mEffect->GetVariableByName("weight1")->AsScalar();
	mWeights[2] = mEffect->GetVariableByName("weight2")->AsScalar();
	mWeights[3] = mEffect->GetVariableByName("weight3")->AsScalar();
	mWeights[4] = mEffect->GetVariableByName("weight4")->AsScalar();
	
	// Set default values
	float defaultWeigths[] = {1.0f, 0.9f, 0.55f, 0.18f, 0.1f};
	SetWeights(defaultWeigths);

	return true;
}
