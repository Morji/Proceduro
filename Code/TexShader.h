#ifndef _H_TEXSHADER
#define _H_TEXSHADER


#include "LightShader.h"
#include "Light.h"

class TexShader : public LightShader
{
public:
	TexShader(void);

	bool Initialize(ID3D10Device* device, HWND hwnd);

	void Render(ID3D10Device* device, int indexCount, 
													  D3DXMATRIX worldMatrix, 
													  D3DXMATRIX viewMatrix, 
													  D3DXMATRIX projectionMatrix,
													  D3DXMATRIX textureMatrix,
													  D3DXVECTOR3 mEyePos, 													  
													  Light lightVar,
													  int lightType,
													  ID3D10ShaderResourceView *diffuseMap,
													  ID3D10ShaderResourceView *specularMap);

	~TexShader(void);

private:

	ID3D10EffectMatrixVariable* mTexMatrix;
	ID3D10EffectShaderResourceVariable* mDiffuseMap;			//for regular texturing
	ID3D10EffectShaderResourceVariable* mSpecularMap;			//for regular and mutli texturing

	void SetShaderParameters(int indexCount, 
							D3DXMATRIX worldMatrix, 
							D3DXMATRIX viewMatrix, 
							D3DXMATRIX projectionMatrix,
							D3DXMATRIX textureMatrix,
							D3DXVECTOR3 mEyePos, 
							Light lightVar,
							int lightType,
							ID3D10ShaderResourceView *diffuseMap,
							ID3D10ShaderResourceView *specularMap);

	bool InitializeShader(ID3D10Device* device, HWND hwnd, WCHAR* filename);
};

#endif