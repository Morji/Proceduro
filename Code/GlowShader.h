#ifndef _GLOWSHADER_H_
#define _GLOWSHADER_H_

#include "IShader.h"

class GlowShader : public IShader
{
public:
	GlowShader(void);
	~GlowShader(void);

	bool Initialize(ID3D10Device* device, HWND hwnd);
	void Render(ID3D10Device* device, int indexCount, ID3D10ShaderResourceView* regularTexture, ID3D10ShaderResourceView* glowTexture, float glowStrength);

private:
	bool InitializeShader(ID3D10Device*, HWND, WCHAR*);
	void SetShaderParameters(ID3D10ShaderResourceView* regularTexture, ID3D10ShaderResourceView* glowTexture, float glowStrength);

private:
	ID3D10EffectShaderResourceVariable*	mRegularTexture;
	ID3D10EffectShaderResourceVariable*	mGlowTexture;
	ID3D10EffectScalarVariable*			mGlowStrength;
};


#endif