#include "TexShader.h"
 

TexShader::TexShader(void)
{
	mTexMatrix = 0;
	mDiffuseMap = 0;
	mSpecularMap = 0;		
}


TexShader::~TexShader(void)	
{
	mTexMatrix = nullptr;
	mDiffuseMap = nullptr;
	mSpecularMap = nullptr;
}

bool TexShader::Initialize(ID3D10Device* device, HWND hwnd){

	bool result;

	result = InitializeShader(device, hwnd, L"Shaders/texture.fx");

	if(!result){
		return false;
	}

	return true;
}

void TexShader::Render(ID3D10Device* device, int indexCount, D3DXMATRIX worldMatrix, 
													  D3DXMATRIX viewMatrix, 
													  D3DXMATRIX projectionMatrix,
													  D3DXMATRIX textureMatrix,
													  D3DXVECTOR3 mEyePos, 
													  Light lightVar,
													  int lightType,
													  ID3D10ShaderResourceView *diffuseMap,
													  ID3D10ShaderResourceView *specularMap)
{

	// Set the shader parameters that it will use for rendering.
	SetShaderParameters(indexCount, worldMatrix, viewMatrix, projectionMatrix, textureMatrix, mEyePos, lightVar, lightType, diffuseMap, specularMap);

	// Now render the prepared buffers with the shader.
	RenderShader(device, indexCount);
}

void TexShader::SetShaderParameters(int indexCount, 
									D3DXMATRIX worldMatrix, 
									D3DXMATRIX viewMatrix, 
									D3DXMATRIX projectionMatrix,
									D3DXMATRIX textureMatrix,
									D3DXVECTOR3 mEyePos, 
									Light lightVar,
									int lightType,
									ID3D10ShaderResourceView *diffuseMap,
									ID3D10ShaderResourceView *specularMap)
{

	LightShader::SetShaderParameters(worldMatrix,viewMatrix,projectionMatrix,mEyePos,lightVar,lightType);

	// Set the eye position variable inside the shader
	mEyePosVar->SetRawValue(&mEyePos, 0, sizeof(D3DXVECTOR3));

	// Set the light variable inside the shader
	mLightVar->SetRawValue(&lightVar, 0, sizeof(Light));

	// Set the light type inside the shader
	mLightType->SetInt(lightType);

	// Set the diffuse map shader var
	mDiffuseMap->SetResource(diffuseMap);

	// Set the specular map shader var
	mSpecularMap->SetResource(specularMap);

	mTexMatrix->SetMatrix((float*)&textureMatrix);
}

bool TexShader::InitializeShader(ID3D10Device* device, HWND hwnd, WCHAR* filename){

	HRESULT result;
	ID3D10Blob* errorMessage;
	
	unsigned int numElements;
	D3D10_PASS_DESC passDesc;

	// Initialize the error message.
	errorMessage = 0;

	//Initialize shader flags
	DWORD shaderFlags = D3D10_SHADER_ENABLE_STRICTNESS;
	#if defined( DEBUG ) || defined( _DEBUG )
		shaderFlags |= D3D10_SHADER_DEBUG;
		shaderFlags |= D3D10_SHADER_SKIP_OPTIMIZATION;
	#endif

	/*Here is where we compile the shader program into an effect. We give it the name of the shader file, 
	the shader version (4.0 in DirectX 10), and the effect to compile the shader into.*/
	// Load the shader in from the file.
	result = D3DX10CreateEffectFromFile(filename, NULL, NULL, "fx_4_0", shaderFlags, 0, 
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

	/*Once the shader code has successfully compiled into an effect we then use that effect to get 
	the technique inside the shader. We will use the technique to draw with the shader from this point forward.*/

	// Get a pointer to the technique inside the shader.
	mTechnique = mEffect->GetTechniqueByName("TextureTechnique");
	if(!mTechnique)
	{
		return false;
	}

	// Now setup the layout of the data that goes into the shader.
	// This setup needs to match the VertexType stucture in the GameObject class and in the shader.
	D3D10_INPUT_ELEMENT_DESC polygonLayout[] = {
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D10_INPUT_PER_VERTEX_DATA, 0},
		{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D10_APPEND_ALIGNED_ELEMENT, D3D10_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D10_APPEND_ALIGNED_ELEMENT, D3D10_INPUT_PER_VERTEX_DATA, 0},
	};
	
	/*Once the layout description has been setup we can get the size of it and then create 
	the input layout using the D3D device.*/

	// Get a count of the elements in the layout.
	numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

	// Get the description of the first pass described in the shader technique.
	mTechnique->GetPassByIndex(0)->GetDesc(&passDesc);

	// Create the input lay9out.
	result = device->CreateInputLayout(polygonLayout, numElements, passDesc.pIAInputSignature, passDesc.IAInputSignatureSize, 
					   &mLayout);
	if(FAILED(result))
	{
		return false;
	}

	/*We will also grab pointers to the global matrices that are inside the shader file. 
	This way when we set a matrix from the main app inside the shader easily by just using these pointers.*/

	// Get pointers to the three matrices inside the shader so we can update them from this class.
	mWorldMatrix =	mEffect->GetVariableByName("worldMatrix")->AsMatrix();
	mViewMatrix =	mEffect->GetVariableByName("viewMatrix")->AsMatrix();
	mProjectionMatrix = mEffect->GetVariableByName("projectionMatrix")->AsMatrix();
	mTexMatrix = mEffect->GetVariableByName("texMatrix")->AsMatrix();

	mEyePosVar		= mEffect->GetVariableByName("gEyePosW");

	mLightVar		= mEffect->GetVariableByName("gLight");
	mLightType		= mEffect->GetVariableByName("gLightType")->AsScalar();

	mDiffuseMap		= mEffect->GetVariableByName("gDiffuseMap")->AsShaderResource();
	mSpecularMap	= mEffect->GetVariableByName("gSpecMap")->AsShaderResource();
	return true;
}