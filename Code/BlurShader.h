////////////////////////////////////////////////////////////////////////////////
// Filename: BlurShader.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _BLURSHADER_H_
#define _BLURSHADER_H_


//////////////
// INCLUDES //
//////////////
#include <d3d10.h>
#include <d3dx10math.h>
#include <fstream>

#include "OrthoTextureShader.h"
using namespace std;


////////////////////////////////////////////////////////////////////////////////
// Class name: BlurShader
////////////////////////////////////////////////////////////////////////////////
class BlurShader : public OrthoTextureShader
{
public:
	BlurShader();
	BlurShader(const BlurShader&);
	~BlurShader();

	bool Initialize(ID3D10Device*, HWND);
	void SetWeights(float weigths[5]);
	void RenderHorizontalBlur(ID3D10Device*, int, ID3D10ShaderResourceView*, float width);
	void RenderVerticalBlur(ID3D10Device*, int, ID3D10ShaderResourceView*, float height);
private:
	bool InitializeShader(ID3D10Device*, HWND, WCHAR*);

private:
	ID3D10EffectScalarVariable* mScreenHeight;
	ID3D10EffectScalarVariable* mScreenWidth;
	ID3D10EffectScalarVariable*	mWeights[5];

	ID3D10InputLayout* mLayoutVertical;
};


#endif