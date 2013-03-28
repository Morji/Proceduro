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

void Renderer::RenderBuffers(ID3D10Device *md3dDevice){
	// Set vertex buffer stride and offset.
	offset = 0;
	// Set the vertex buffer to active in the input assembler so it can be rendered.
	md3dDevice->IASetVertexBuffers(0, 1, &mVB, &stride, &offset);

	// Set the index buffer to active in the input assembler so it can be rendered.
	md3dDevice->IASetIndexBuffer(mIB, DXGI_FORMAT_R32_UINT, 0);

	// Set the type of primitive that should be rendered from this vertex buffer, in this case triangles.
	md3dDevice->IASetPrimitiveTopology(mTopologyChoice);
}
