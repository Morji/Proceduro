#include "Renderer.h"

Renderer::~Renderer(){
	// Release the index buffer.
	ReleaseCOM(mIB);
	// Release the vertex buffer.
	ReleaseCOM(mVB);
}

DWORD Renderer::GetIndexCount(){
	return mIndexCount;
}

DWORD Renderer::GetVertexCount(){
	return mVertexCount;
}

ID3D10Buffer *Renderer::GetVertexBuffer(){
	//only allow access to the vertex buffer if in dynamic access mode
	if (!mDynamic){
		return 0;
	}
	return mVB;
}

void Renderer::SetDynamic(bool isDynamic){
	mDynamic = isDynamic;
}

bool Renderer::InitializeBuffers(ID3D10Device *md3dDevice, DWORD* indices, void *vertices, DWORD stride, DWORD vertexCount, DWORD indexCount){
	mVertexCount = vertexCount;
	mIndexCount = indexCount;

	D3D10_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D10_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;

	// Set up the description of the vertex buffer.
	if (mDynamic){
		vertexBufferDesc.Usage = D3D10_USAGE_DYNAMIC;
		vertexBufferDesc.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
	}
	else{
		vertexBufferDesc.Usage = D3D10_USAGE_DEFAULT;
		vertexBufferDesc.CPUAccessFlags = 0;
	}
	vertexBufferDesc.ByteWidth = stride * mVertexCount;
	vertexBufferDesc.BindFlags = D3D10_BIND_VERTEX_BUFFER;	
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

	mStride = stride; //set the stride of the buffers to be the size of the vertex struct

	return true;
}

void Renderer::RenderBuffers(ID3D10Device *md3dDevice){
	// Set vertex buffer stride and offset.
	mOffset = 0;
	// Set the vertex buffer to active in the input assembler so it can be rendered.
	md3dDevice->IASetVertexBuffers(0, 1, &mVB, &mStride, &mOffset);

	// Set the index buffer to active in the input assembler so it can be rendered.
	md3dDevice->IASetIndexBuffer(mIB, DXGI_FORMAT_R32_UINT, 0);

	// Set the type of primitive that should be rendered from this vertex buffer, in this case triangles.
	md3dDevice->IASetPrimitiveTopology(mTopologyChoice);
}
