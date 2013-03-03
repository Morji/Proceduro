#include "Tree.h"

Tree::Tree(ID3D10Device *d3dDevice, HWND hwnd, Vector3f position){
	md3dDevice = d3dDevice;
	mTreePosition = position;
	treeShader = new TexShader();
	if (!treeShader->Initialize(md3dDevice,hwnd)){
		MessageBox(hwnd, L"Error while creating tree shader", L"Error", MB_OK);
	}

	newTreeShader = new TreeShader();
	if (!newTreeShader->Initialize(md3dDevice,hwnd)){
		MessageBox(hwnd, L"Error while creating new tree shader", L"Error", MB_OK);
	}

	treeTexture = new TextureLoader();
	treeTexture->Initialize(md3dDevice,L"assets/textures/tree_texture.jpg");

	mVB = 0;
	mIB = 0;
	mVertexCount = mIndexCount = 0;
}


Tree::~Tree(void){
	// Release the index buffer.
	ReleaseCOM(mIB);
	// Release the vertex buffer.
	ReleaseCOM(mVB);
	while (!treeSegmentList.empty()){
		TreeSegment *segment = treeSegmentList.back();
		if (segment){
			delete segment;
			segment = nullptr;
		}
		treeSegmentList.pop_back();
	}
	treeSegmentList.clear();
	if (treeShader){
		delete treeShader;
		treeShader = nullptr;
	}
	if (newTreeShader){
		delete newTreeShader;
		newTreeShader = nullptr;
	}
	if (treeTexture){
		treeTexture->Shutdown();
		delete treeTexture;
		treeTexture = nullptr;
	}
	md3dDevice = 0;
}

bool Tree::InitializeBuffers(DWORD* indices, TreeVertex* vertices){

	D3D10_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D10_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;

	// Set up the description of the vertex buffer.
	vertexBufferDesc.Usage = D3D10_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(TreeVertex) * mVertexCount;
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

	stride = sizeof(TreeVertex); //set the stride of the buffers to be the size of the vertex struct

	return true;
}

void Tree::RenderTreeBuffers(){
	// Set vertex buffer stride and offset.
	offset = 0;
	// Set the vertex buffer to active in the input assembler so it can be rendered.
	md3dDevice->IASetVertexBuffers(0, 1, &mVB, &stride, &offset);

	// Set the index buffer to active in the input assembler so it can be rendered.
	md3dDevice->IASetIndexBuffer(mIB, DXGI_FORMAT_R32_UINT, 0);

	// Set the type of primitive that should be rendered from this vertex buffer, in this case triangles.
	md3dDevice->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_POINTLIST);
}

void Tree::Render(D3DXMATRIX worldMatrix,D3DXMATRIX viewMatrix,D3DXMATRIX projectionMatrix, Vector3f eyePos, Light light, int lightType){
	D3DXMATRIX m,mObjMatrix;
	D3DXMatrixIdentity(&mObjMatrix);
	D3DXMatrixTranslation(&m, mTreePosition.x, mTreePosition.y, mTreePosition.z);
	mObjMatrix *= m;
	mObjMatrix *= worldMatrix;

	RenderTreeBuffers();
	newTreeShader->Render(md3dDevice,mIndexCount,mObjMatrix,viewMatrix,projectionMatrix,eyePos,light,lightType);


}

bool Tree::GenerateTreeSpaceExploration(float minGrowthSize, float startRadius, float maxHeight){

	const int targetPosSize = 800;
	const float sphereScale = maxHeight*0.5f;

	float minInfluenceDist = 4.0f*minGrowthSize;
	float maxInfluenceDist = 15.0f*minGrowthSize;
	bool endGrowth = false;	//wether or not the algorithm has finished

	//generate tree trunk to half of maxHeight size
	int trunkChunks = (int)((maxHeight*0.5f)/minGrowthSize);
	
	for (int i = 0; i < trunkChunks; i++){
		TreeSegment *segment = new TreeSegment;
		segment->SetPosition(Vector3f(sphereScale/2.0f,i*minGrowthSize,sphereScale/2.0f));
		segment->SetHeight(minGrowthSize);
		segment->SetRadius(startRadius*2);
		if (i > 0){
			segment->SetParent(treeSegmentList[i-1]);
		}
		treeSegmentList.push_back(segment);
	}

	//generate growth target positions - for now we are still dealing in local space so no need to change to world space
	std::vector<Vector3f> growthTargetHolder;	
	for (int i = 0; i < targetPosSize; i++){
		Vector3f growthTargetPosition;
		growthTargetPosition = RandUnitVecInside3();	
		
		growthTargetPosition *= sphereScale;
		growthTargetPosition.y += sphereScale;

		growthTargetHolder.push_back(growthTargetPosition);	
	}

	//start algorithm
	while (!endGrowth){
		//compare all growth targets to the branches(segments)
		for (int i = 0; i < growthTargetHolder.size(); i++){
			Vector3f growthTargetPos = growthTargetHolder[i];
			TreeSegment *closestSegment = nullptr;
			float closestDistance = maxInfluenceDist;

			for (int j = 0; j < treeSegmentList.size(); j++){
				TreeSegment *treeSegment = treeSegmentList[j];
				//get the directional vector from the growth target to this branch and it's length
				Vector3f branchPos;
				treeSegment->GetPosition(branchPos);
				Vector3f dirVector = growthTargetPos - branchPos;
				float distance = D3DXVec3Length(&dirVector);
				//if the distance to this branch is over the maximum influence distance, check the next one
				if (distance > maxInfluenceDist){
					continue;
				}
				//if the distance to this branch is within the minimum influence distance, remove this growth target
				else if (distance <= minInfluenceDist){
					growthTargetHolder.erase(growthTargetHolder.begin() + i);
					break;
				}
				//if this distance is less than the closestDistance currently achieved, mark that this is the closest segment
				if (distance < closestDistance) {
					closestSegment = treeSegment;	
					closestDistance = distance;
				}
			}
			//if a closest segment has been found, modify its growth direction(this will increment it's growth count)
			if (closestSegment != nullptr){	
				Vector3f branchPos;
				closestSegment->GetPosition(branchPos);
				Vector3f growthDirection = growthTargetPos - branchPos;
				D3DXVec3Normalize(&growthDirection,&growthDirection);
				closestSegment->IncrementGrowthDir(growthDirection);				
			}
		}

		endGrowth = true;

		//check branches(segments) for growth
		for (int i = 0; i < treeSegmentList.size(); i++){
			TreeSegment *treeSegment = treeSegmentList[i];			
			int growthCount = treeSegment->GetGrowthCount();
			//if this branch has been marked for growth
			if (growthCount > 0){
				endGrowth = false;//algorithm still running
				//get the average growth direction
				Vector3f growthDirection;
				treeSegment->GetGrowthDirection(growthDirection);
				growthDirection /= growthCount;
				D3DXVec3Normalize(&growthDirection,&growthDirection);
				
				//spawn new branch(segment)
				Vector3f segmentPosition;
				treeSegment->GetPosition(segmentPosition);
				Vector3f newSegmentPosition = segmentPosition + (growthDirection*minGrowthSize);	

				TreeSegment *newSegment = new TreeSegment;
				newSegment->SetPosition(newSegmentPosition);
				newSegment->SetHeight(minGrowthSize);
				newSegment->SetRadius(startRadius*2);
				newSegment->SetParent(treeSegment);
				treeSegmentList.push_back(newSegment);

				//increment the growth of all parents
				TreeSegment *parent = treeSegment;
				while (parent){
					parent->IncrementRadius(startRadius);
					parent = parent->GetParent();
				}

				//reset growth of old segment
				treeSegment->ResetGrowth();
			}
		}
	}

	//clean up
	while (!growthTargetHolder.empty()){
		growthTargetHolder.pop_back();
	}
	growthTargetHolder.clear();

	mVertexCount = mIndexCount = treeSegmentList.size();

	TreeVertex *vertices = new TreeVertex[mVertexCount];
	for (int i = 0; i < mVertexCount; i++){
		treeSegmentList[i]->GetPosition(vertices[i].pos);
		vertices[i].radius = treeSegmentList[i]->GetRadius();
		TreeSegment *parent = treeSegmentList[i]->GetParent();
		if (parent){
			parent->GetPosition(vertices[i].parent);
			vertices[i].parentRadius = parent->GetRadius();
		}
		else{
			vertices[i].parent = vertices[i].pos;
			vertices[i].parentRadius = vertices[i].radius;
		}
	}

	DWORD	*indices = new DWORD[mIndexCount];
	for (int i = 0; i < mIndexCount; i++){
		indices[i] = i;
	}

	
	if (!InitializeBuffers(indices,vertices)){
		return false;
	}


	return true;
}
