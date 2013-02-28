////////////////////////////////////////////////////////////////////////////////
// Filename: SimpleTextureShader.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _SimpleTextureShader_H_
#define _SimpleTextureShader_H_


//////////////
// INCLUDES //
//////////////
#include <d3d10.h>
#include <d3dx10.h>
#include <fstream>

#include "IShader.h"
using namespace std;


////////////////////////////////////////////////////////////////////////////////
// Class name: SimpleTextureShader
////////////////////////////////////////////////////////////////////////////////
class SimpleTextureShader : public IShader
{
public:
	SimpleTextureShader();
	SimpleTextureShader(const SimpleTextureShader&);
	~SimpleTextureShader();

	bool Initialize(ID3D10Device*, HWND);
	void Render(ID3D10Device*, int, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, ID3D10ShaderResourceView*);

private:
	bool InitializeShader(ID3D10Device*, HWND, WCHAR*);

	void SetShaderParameters(D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, ID3D10ShaderResourceView*);

private:
	ID3D10EffectMatrixVariable* mWorldMatrix;
	ID3D10EffectMatrixVariable* mViewMatrix;
	ID3D10EffectMatrixVariable* mProjectionMatrix;
	ID3D10EffectShaderResourceVariable* mTexture;
};

#endif