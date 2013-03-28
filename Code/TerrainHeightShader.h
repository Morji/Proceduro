#ifndef _H_TerrainHeightShader
#define _H_TerrainHeightShader

///SHADER THAT HANDLES MULTITEXTURING

#include "LightShader.h"
#include "Light.h"


class TerrainHeightShader : public LightShader
{
public:
	TerrainHeightShader(void);

	bool Initialize(ID3D10Device* device, HWND hwnd);

	void SetShaderParameters(ID3D10Device* device, D3DXMATRIX worldMatrix, 
											D3DXMATRIX viewMatrix, 
											D3DXMATRIX projectionMatrix,
											D3DXVECTOR3 mEyePos, 
											Light lightVar,
											ID3D10ShaderResourceView* diffuseMapRV1,
											ID3D10ShaderResourceView* diffuseMapRV2,
											ID3D10ShaderResourceView* diffuseMapRV3,
											float maxHeight,
											int lightType);

	void Render(ID3D10Device* device, int indexCount);
	~TerrainHeightShader(void);

private:

	ID3D10EffectScalarVariable*			mHeights[3];			//for height-mapped multi texturing	

	ID3D10EffectShaderResourceVariable* mDiffuseMapRV[3];		

	

	bool InitializeShader(ID3D10Device* device, HWND hwnd, WCHAR* filename);
};

#endif