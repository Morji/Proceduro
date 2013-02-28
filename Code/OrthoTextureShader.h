////////////////////////////////////////////////////////////////////////////////
// Filename: OrthoTextureShader.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _OrthoTextureShader_H_
#define _OrthoTextureShader_H_


//////////////
// INCLUDES //
//////////////
#include <d3d10.h>
#include <d3dx10.h>
#include <fstream>

#include "IShader.h"

using namespace std;


////////////////////////////////////////////////////////////////////////////////
// Class name: OrthoTextureShader
////////////////////////////////////////////////////////////////////////////////
class OrthoTextureShader : public IShader
{
public:
	OrthoTextureShader();
	OrthoTextureShader(const OrthoTextureShader&);
	~OrthoTextureShader();

	bool Initialize(ID3D10Device*, HWND);
	void Render(ID3D10Device*, int, ID3D10ShaderResourceView*);

private:
	bool InitializeShader(ID3D10Device*, HWND, WCHAR*);	

protected:
	void SetShaderParameters(ID3D10ShaderResourceView*);

protected:
	ID3D10EffectShaderResourceVariable* mTexture;	
};

#endif