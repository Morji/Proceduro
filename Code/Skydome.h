/*************************************************************
Skydome Class: Encapsulates a Skydome object which is a sphere
that moves with the camera

Author: Valentin Hinov
Date: 09/04/2013
Version: 1.0

Requires: BaseGameObject.h
Exposes: Skydome
**************************************************************/

#include "BaseGameObject.h"
#include "Vertex.h"
#include "Renderer.h"
#include "Transform.h"
#include "SkyShader.h"
#include "ID3DObject.h"

#ifndef _H_SKYDOME_H
#define _H_SKYDOME_H

class Skydome : public BaseGameObject{
public:
	Skydome(void);
	~Skydome(void);

	bool Initialize(ID3D10Device* device, HWND hwnd);
	void Render(ID3DObject* d3dObject, D3DXMATRIX viewMatrix);
	void Update(float dt);

// Components
private:
	Transform	*mTransform;
	Renderer	*mSkyRenderer;

private:
	SkyShader	*mSkyShader;	
	Color		mApexColor;
	Color		mCenterColor;

	bool		day;
};

#endif