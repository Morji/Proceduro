////////////////////////////////////////////////////////////////////////////////
// Filename: VerticalBlurShader.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _VerticalBLURSHADER_H_
#define _VerticalBLURSHADER_H_


//////////////
// INCLUDES //
//////////////
#include <d3d10.h>
#include <d3dx10math.h>
#include <fstream>

#include "OrthoTextureShader.h"
using namespace std;


////////////////////////////////////////////////////////////////////////////////
// Class name: VerticalBlurShader
////////////////////////////////////////////////////////////////////////////////
class VerticalBlurShader : public OrthoTextureShader
{
public:
	VerticalBlurShader();
	VerticalBlurShader(const VerticalBlurShader&);
	~VerticalBlurShader();

	bool Initialize(ID3D10Device*, HWND);
	void Render(ID3D10Device*, int, ID3D10ShaderResourceView*, float);

private:
	bool InitializeShader(ID3D10Device*, HWND, WCHAR*);

	void SetShaderParameters(ID3D10ShaderResourceView*, float);

private:
	ID3D10EffectScalarVariable* mScreenHeight;
};


#endif