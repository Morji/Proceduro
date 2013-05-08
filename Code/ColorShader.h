#ifndef _COLORSHADER_H_
#define _COLORSHADER_H_

#include "IShader.h"

class ColorShader : public IShader
{
public:
	ColorShader(void);
	~ColorShader(void);

	bool Initialize(ID3D10Device* device, HWND hwnd);
	void Render(ID3D10Device* device, int indexCount, D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix, D3DXMATRIX projectionMatrix);

private:
	bool InitializeShader(ID3D10Device*, HWND, WCHAR*);
	void SetShaderParameters(D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix, D3DXMATRIX projectionMatrix);

protected:
	ID3D10EffectMatrixVariable* mWorldMatrix;
	ID3D10EffectMatrixVariable* mViewMatrix;
	ID3D10EffectMatrixVariable* mProjectionMatrix;
};


#endif