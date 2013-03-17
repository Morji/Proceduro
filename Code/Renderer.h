/*************************************************************
Renderer Class: Provides a render component for game objects.

Author: Valentin Hinov
Date: 17/03/2013
Version: 1.0

Exposes: Renderer
**************************************************************/

#ifndef _H_RENDERER_H
#define _H_RENDERER_H

#include "IComponent.h"
#include "d3dUtil.h"


class Renderer : public IComponent{
public:
	Renderer(D3D_PRIMITIVE_TOPOLOGY topologyChoice): mVB(0), mIB(0),stride(0),offset(0),mVertexCount(0),mIndexCount(0){
		mTopologyChoice = topologyChoice;
	}

	template <typename V>
	bool InitializeBuffers(ID3D10Device	*md3dDevice, DWORD* indices, V *vertices, DWORD vertexCount, DWORD indexCount);
	void RenderBuffers(ID3D10Device	*md3dDevice);

	DWORD GetIndexCount();
	DWORD GetVertexCount();

	~Renderer();
private:
	ID3D10Buffer	*mVB;
	ID3D10Buffer	*mIB;
	unsigned int	stride;
	unsigned int	offset;
	DWORD			mVertexCount;
	DWORD			mIndexCount;

	D3D_PRIMITIVE_TOPOLOGY	mTopologyChoice;
};


template <typename V>
bool Renderer::InitializeBuffers(ID3D10Device *md3dDevice, DWORD* indices, V *vertices,  DWORD vertexCount, DWORD indexCount){

	mVertexCount = vertexCount;
	mIndexCount = indexCount;

	D3D10_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D10_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;

	// Set up the description of the vertex buffer.
	vertexBufferDesc.Usage = D3D10_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(V) * mVertexCount;
	vertexBufferDesc.BindFlags = D3D10_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;

	// Give the subresource structure a pointer to the vertex data.
	vertexData.pSysMem = vertices;

	// Now finally create the vertex buffer.
	result = md3dDevice->CreateBuffer(&vertexBufferDesc, &vertexData, &mVB);
	if(FAILED(result))
	{
		return false;
	}

	// Set up the description of the index buffer.
	indexBufferDesc.Usage = D3D10_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(DWORD) * mIndexCount;
	indexBufferDesc.BindFlags = D3D10_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;

	// Give the subresource structure a pointer to the index data.
	indexData.pSysMem = indices;

	// Create the index buffer.
	result = md3dDevice->CreateBuffer(&indexBufferDesc, &indexData, &mIB);

	if(FAILED(result)){
		return false;
	}

	stride = sizeof(V); //set the stride of the buffers to be the size of the vertex struct

	return true;
}

#endif