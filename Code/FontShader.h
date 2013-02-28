////////////////////////////////////////////////////////////////////////////////
// Filename: FontShader.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _FontShader_H_
#define _FontShader_H_


//////////////
// INCLUDES //
//////////////
#include <d3d10.h>
#include <d3dx10.h>
#include <fstream>

#include "IShader.h"
using namespace std;


////////////////////////////////////////////////////////////////////////////////
// Class name: FontShader
////////////////////////////////////////////////////////////////////////////////
class FontShader : public IShader
{
public:
	FontShader();
	FontShader(const FontShader&);
	~FontShader();

	bool Initialize(ID3D10Device*, HWND);
	void Render(ID3D10Device*, int, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, ID3D10ShaderResourceView*, D3DXVECTOR4);

private:
	bool InitializeShader(ID3D10Device*, HWND, WCHAR*);

	void SetShaderParameters(D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, ID3D10ShaderResourceView*, D3DXVECTOR4);

private:
	ID3D10EffectMatrixVariable* mWorldMatrix;
	ID3D10EffectMatrixVariable* mViewMatrix;
	ID3D10EffectMatrixVariable* mProjectionMatrix;
	ID3D10EffectShaderResourceVariable* mTexture;
	ID3D10EffectVectorVariable* mPixelColor;
};

#endif