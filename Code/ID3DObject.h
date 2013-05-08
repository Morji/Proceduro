/*************************************************************
ID3DObject Class: Provides an interface to some
directx graphics functionalities to be passed to renderable
objects.


Author: Valentin Hinov
Date: 05/05/2013
Version: 1.0

Exposes: ID3DObject
**************************************************************/

#ifndef _ID3DOBJECT_H
#define _ID3DOBJECT_H

#include "d3dUtil.h"

class ID3DObject{
public:
	ID3DObject(){}
	virtual ~ID3DObject(){}

	virtual ID3D10Device* GetDevice() = 0;
	virtual D3D10_VIEWPORT	*GetViewport() = 0;

	virtual D3DXMATRIX *GetProjectionMatrix() = 0;
	virtual D3DXMATRIX *GetWorldMatrix() = 0;
	virtual D3DXMATRIX *GetOrthoMatrix() = 0;

	virtual void GetProjectionMatrix(D3DXMATRIX&) = 0;
	virtual void GetWorldMatrix(D3DXMATRIX&) = 0;
	virtual void GetOrthoMatrix(D3DXMATRIX&) = 0;

	virtual void GetScreenDimensions(int *width, int *height) = 0;
	virtual void GetScreenDepthInfo(float *nearVal, float *farVal) = 0;

	virtual void SetBackBufferRenderTarget() = 0;
	virtual void ResetViewport() = 0;

	virtual void TurnZBufferOn() = 0;
	virtual void TurnZBufferOff() = 0;
	virtual void TurnOnAlphaBlending() = 0;
	virtual void TurnOffAlphaBlending() = 0;

	virtual void TurnWireframeOn() = 0;
	virtual void TurnWireframeOff() = 0;
 
	virtual void TurnCullingOn() = 0;
	virtual void TurnCullingOff() = 0;

	virtual bool Screenshot() = 0;


};

#endif