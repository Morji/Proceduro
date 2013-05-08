#include "SunShader.h"


SunShader::SunShader(void){
	mWorldMatrix = nullptr;
	mViewMatrix = nullptr;
	mProjectionMatrix = nullptr;
	mNormalColor = nullptr;
	mGlowColor = nullptr;
}


SunShader::~SunShader(void){
	mWorldMatrix = nullptr;
	mViewMatrix = nullptr;
	mProjectionMatrix = nullptr;
	mNormalColor = nullptr;
	mGlowColor = nullptr;
}

bool SunShader::Initialize(ID3D10Device* device, HWND hwnd){
	bool result;

	// Initialize the vertex and pixel shaders.
	result = InitializeShader(device, hwnd, L"Shaders/suneffect.fx");
	if(!result){
		return false;
	}

	return true;
}

void SunShader::Render(ID3D10Device* device, int indexCount, D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix, D3DXMATRIX projectionMatrix, Color normalColor, Color glowColor){
	// Set the parameters inside the shader
	SetShaderParameters(worldMatrix,viewMatrix,projectionMatrix,normalColor,glowColor);

	// Now render the prepared buffers with the shader.
	RenderShader(device, indexCount);
}

void SunShader::SetShaderParameters(D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix, D3DXMATRIX projectionMatrix, Color normalColor, Color glowColor){
	// Set the world matrix variable inside the shader.
    mWorldMatrix->SetMatrix((float*)&worldMatrix);

	// Set the view matrix variable inside the shader.
	mViewMatrix->SetMatrix((float*)&viewMatrix);

	// Set the projection matrix variable inside the shader.
    mProjectionMatrix->SetMatrix((float*)&projectionMatrix);

	// Set the apex color vector variable
	mNormalColor->SetFloatVector((float*)&normalColor);

	// Set the center color vector variable
	mGlowColor->SetFloatVector((float*)&glowColor);

}

bool SunShader::InitializeShader(ID3D10Device* device, HWND hwnd, WCHAR* filename){
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
	mTechnique = mEffect->GetTechniqueByName("SunTechnique");
	if(!mTechnique)
	{
		return false;
	}

	D3D10_INPUT_ELEMENT_DESC polygonLayout[1];

	// Now setup the layout of the data that goes into the shader.
	// This setup needs to match the VertexType stucture in the ModelClass and in the shader.
	polygonLayout[0].SemanticName = "POSITION";
	polygonLayout[0].SemanticIndex = 0;
	polygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[0].InputSlot = 0;
	polygonLayout[0].AlignedByteOffset = 0;
	polygonLayout[0].InputSlotClass = D3D10_INPUT_PER_VERTEX_DATA;
	polygonLayout[0].InstanceDataStepRate = 0;

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
	mWorldMatrix		= mEffect->GetVariableByName("worldMatrix")->AsMatrix();
	mViewMatrix			= mEffect->GetVariableByName("viewMatrix")->AsMatrix();
	mProjectionMatrix	= mEffect->GetVariableByName("projectionMatrix")->AsMatrix();

	mNormalColor		= mEffect->GetVariableByName("normalColor")->AsVector();
	mGlowColor			= mEffect->GetVariableByName("glowColor")->AsVector();

	return true;
}