////////////////////////////////////////////////////////////////////////////////
// Filename: RetainBrightnessShader.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _RETAINBRIGHTNESSSHADER_H_
#define _RETAINBRIGHTNESSSHADER_H_


//////////////
// INCLUDES //
//////////////
#include <d3d10.h>
#include <d3dx10math.h>
#include <fstream>

#include "OrthoTextureShader.h"
using namespace std;


////////////////////////////////////////////////////////////////////////////////
// Class name: RetainBrightnessShader
////////////////////////////////////////////////////////////////////////////////
class RetainBrightnessShader : public OrthoTextureShader
{
public:
	RetainBrightnessShader();
	RetainBrightnessShader(const RetainBrightnessShader&);
	~RetainBrightnessShader();

	bool Initialize(ID3D10Device*, HWND);
	void Render(ID3D10Device*, int, ID3D10ShaderResourceView*, float luminanceThreshold);
private:
	bool InitializeShader(ID3D10Device*, HWND, WCHAR*);

private:
	ID3D10EffectScalarVariable *mLuminanceThreshold;
};


#endif