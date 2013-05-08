#ifndef _H_WATERSHADER_H
#define _H_WATERSHADER_H

#include "LightShader.h"

class WaterShader : public LightShader
{
public:
	WaterShader();

	~WaterShader();

	bool Initialize(ID3D10Device*, HWND);

	void SetShaderParameters(D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix, D3DXMATRIX projectionMatrix, 
				D3DXVECTOR3 mEyePos, Light lightVar, int lightType);

	void Render(ID3D10Device* device, int indexCount);

private:
	bool InitializeShader(ID3D10Device*, HWND, WCHAR*);

};

#endif