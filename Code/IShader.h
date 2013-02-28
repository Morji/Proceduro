/*************************************************************
IShader Class: Provides an interface for the initialization
and clean-up of shader objects as well as some utility
functions such as OutputShaderError and RenderShader

Author: Valentin Hinov
Date: 28/02/2013
Version: 1.0

Exposes: ISHADER
**************************************************************/

#ifndef _ISHADER_H_
#define _ISHADER_H_

#include "d3dUtil.h"
#include <fstream>

class IShader
{
public:
	IShader(void){
		mEffect = 0;
		mTechnique = 0;
		mLayout = 0;
	}

	/*The functions here handle initializing and shutdown of the shader. 
	The render function sets the shader parameters and then draws the 
	prepared model vertices using the shader.*/
	virtual bool Initialize(ID3D10Device* device, HWND hwnd) = 0;
	

	~IShader(void){
		if (mEffect){
			mEffect->Release();
			mEffect = nullptr;
		}
		if (mTechnique){
			mTechnique = nullptr;
		}
		if (mLayout){
			mLayout->Release();
			mLayout = nullptr;
		}
	}

private:
	virtual bool InitializeShader(ID3D10Device* device, HWND hwnd, WCHAR* filename) = 0;

protected:	
	void OutputShaderErrorMessage(ID3D10Blob* errorMessage, HWND hwnd, WCHAR* shaderFilename);
	void RenderShader(ID3D10Device* device, int indexCount);

protected:
	ID3D10Effect* mEffect;
	ID3D10EffectTechnique* mTechnique;
	ID3D10InputLayout* mLayout;
};


#endif