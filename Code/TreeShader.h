/*************************************************************
TreeShader Class: Used to draw and create a tree shape object
on the GPU

Author: Valentin Hinov
Date: 28/02/2013
Version: 1.0

Exposes: TreeShader
**************************************************************/
#ifndef _TreeShader_H_
#define _TreeShader_H_


//////////////
// INCLUDES //
//////////////
#include <d3d10.h>
#include <d3dx10.h>
#include <fstream>

#include "LightShader.h"
using namespace std;


////////////////////////////////////////////////////////////////////////////////
// Class name: TreeShader
////////////////////////////////////////////////////////////////////////////////
class TreeShader : public LightShader
{
public:
	TreeShader();
	TreeShader(const TreeShader&);
	~TreeShader();

	bool Initialize(ID3D10Device*, HWND);
	void Render(ID3D10Device* device, int indexCount, D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix, D3DXMATRIX projectionMatrix, 
				D3DXVECTOR3 mEyePos, Light lightVar, int lightType);

private:
	bool InitializeShader(ID3D10Device*, HWND, WCHAR*);

	void SetShaderParameters(D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix, D3DXMATRIX projectionMatrix, 
				D3DXVECTOR3 mEyePos, Light lightVar, int lightType);

private:	

};

#endif