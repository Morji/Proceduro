#include "Vertex.h"

#ifndef _H_SUNSHADER_H
#define _H_SUNSHADER_H

#include "IShader.h"

class SunShader : public IShader{
public:
	SunShader(void);
	~SunShader(void);

	bool Initialize(ID3D10Device* device, HWND hwnd);

	void Render(ID3D10Device* device, int indexCount, D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix, D3DXMATRIX projectionMatrix, Color normalColor, Color glowColor);

private:
	bool InitializeShader(ID3D10Device*, HWND, WCHAR*);
	void SetShaderParameters(D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix, D3DXMATRIX projectionMatrix, Color normalColor, Color glowColor);

private:
	ID3D10EffectMatrixVariable* mWorldMatrix;
	ID3D10EffectMatrixVariable* mViewMatrix;
	ID3D10EffectMatrixVariable* mProjectionMatrix;

	ID3D10EffectVectorVariable* mNormalColor;
	ID3D10EffectVectorVariable* mGlowColor;
};

#endif