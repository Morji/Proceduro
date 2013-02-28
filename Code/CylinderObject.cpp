#include "CylinderObject.h"

CylinderObject::CylinderObject(unsigned int numberOfSegments, float radius, float height){
	mNumberOfSegments = numberOfSegments;
	mRadius = radius;
	mHeight = height;
	D3DXMatrixIdentity(&mRotationMatrix);

	mTopologyChoice = D3D10_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;

	mVertexCount = mNumberOfSegments*2;	
	mIndexCount = mVertexCount+2;
}

CylinderObject::~CylinderObject(void){
	Shutdown();	
}

void CylinderObject::setTrans(D3DXMATRIX worldMatrix){
	D3DXMATRIX m;
	D3DXMatrixIdentity(&objMatrix);
	D3DXMatrixScaling(&m, scale.x, scale.y, scale.z);
	objMatrix *= mRotationMatrix;
	objMatrix *= m;
	D3DXMatrixTranslation(&m, pos.x, pos.y, pos.z);
	objMatrix*=m;
	objMatrix *= worldMatrix;
}

void CylinderObject::SetRotation(D3DXQUATERNION *quaternion){
	D3DXMatrixRotationQuaternion(&mRotationMatrix,quaternion);
}

void CylinderObject::SetRadius(float radius){
	this->mRadius = radius;
}

void CylinderObject::SetHeight(float height){
	this->mHeight = height;
}

bool CylinderObject::SetupArraysAndInitBuffers(){
	
	DWORD *indices = new DWORD[mIndexCount];
	VertexNT	*vertices = new VertexNT[mVertexCount];

	float rDeltaSegAngle = (2.0f * PI / mNumberOfSegments);
	float rSegmentLength = 1.0f / (float)mNumberOfSegments;

	int currVert = 0;

	for (int currSegment = 0; currSegment < mNumberOfSegments; currSegment++){
		  float x0 = mRadius * sinf(currSegment * rDeltaSegAngle);
		  float z0 = mRadius * cosf(currSegment * rDeltaSegAngle);

		  vertices[currVert].pos = Vector3f(x0,(mHeight / 2.0f),z0);
		  vertices[currVert].normal = Vector3f(x0,0.0f,z0);
		  vertices[currVert].texC = Vector2f(1.0f - (rSegmentLength * (float)currSegment), 0.0f);
		  currVert++;

		  vertices[currVert].pos = Vector3f(x0,(-mHeight / 2.0f),z0);
		  vertices[currVert].normal = Vector3f(x0,0.0f,z0);
		  vertices[currVert].texC = Vector2f(1.0f - (rSegmentLength * (float)currSegment), 1.0f);
		  currVert++;
	}

	for (unsigned long i = 0; i < mIndexCount; i++){
		indices[i] = (i % mVertexCount);
	}

	if (!InitializeBuffers(indices,vertices)){
		return false;
	}
	
	if (indices){
		delete [] indices;
		indices = nullptr;
	}
	if (vertices){
		delete [] vertices;
		vertices = nullptr;
	}

	return true;
}

bool CylinderObject::InitializeBuffers(DWORD* indices,  VertexNT* vertices){

	D3D10_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D10_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;

	// Set up the description of the vertex buffer.
	vertexBufferDesc.Usage = D3D10_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexNT) * mVertexCount;
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

	stride = sizeof(VertexNT); //set the stride of the buffers to be the size of the vertexNT

	return true;
}