#include "Vertex.h"

#ifndef _H_SKYSHADER_H
#define _H_SKYSHADER_H

#include "IShader.h"

class SkyShader : public IShader{
public:
	SkyShader(void);
	~SkyShader(void);

	bool Initialize(ID3D10Device* device, HWND hwnd);

	void Render(ID3D10Device* device, int indexCount, D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix, D3DXMATRIX projectionMatrix, Color apexColor, Color centerColor);

private:
	bool InitializeShader(ID3D10Device*, HWND, WCHAR*);
	void SetShaderParameters(D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix, D3DXMATRIX projectionMatrix, Color apexColor, Color centerColor);

private:
	ID3D10EffectMatrixVariable* mWorldMatrix;
	ID3D10EffectMatrixVariable* mViewMatrix;
	ID3D10EffectMatrixVariable* mProjectionMatrix;

	ID3D10EffectVectorVariable* mApexColor;
	ID3D10EffectVectorVariable* mCenterColor;
};

#endif