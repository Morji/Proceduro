/*************************************************************
Renderer Class: Provides a render component for game objects.

Author: Valentin Hinov
Date: 17/03/2013
Version: 1.0

Exposes: Renderer
**************************************************************/

#ifndef _H_RENDERER_H
#define _H_RENDERER_H

#include "Component.h"
#include "d3dUtil.h"


class Renderer : public Component{
public:
	//default renderer to a triangle list
	Renderer(BaseGameObject *parent = nullptr, D3D_PRIMITIVE_TOPOLOGY topologyChoice = D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST): Component(parent),
	  mVB(0), mIB(0),mStride(0),mOffset(0),mVertexCount(0),mIndexCount(0),mDynamic(false){
		mTopologyChoice = topologyChoice;
	}

	void RenderBuffers(ID3D10Device	*md3dDevice);

	bool InitializeBuffers(ID3D10Device *md3dDevice, DWORD* indices, void *vertices, DWORD stride, DWORD vertexCount, DWORD indexCount);

	void SetDynamic(bool isDynamic);

	ID3D10Buffer *GetVertexBuffer();

	DWORD GetIndexCount();
	DWORD GetVertexCount();

	~Renderer();
private:
	ID3D10Buffer	*mVB;
	ID3D10Buffer	*mIB;
	unsigned int	mStride;
	unsigned int	mOffset;
	DWORD			mVertexCount;
	DWORD			mIndexCount;

	bool			mDynamic;

	D3D_PRIMITIVE_TOPOLOGY	mTopologyChoice;
};

#endif