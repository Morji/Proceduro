#include "Tree.h"

#include "ValMath.h"

Tree::Tree(Vector3f position){
	mTransform = new Transform(this);
	mTransform->position = position;
	mBoundingBox = new BoundingBox(this);
	mRenderer = new Renderer(this,D3D10_PRIMITIVE_TOPOLOGY_POINTLIST);
	AddComponent(mTransform,TRANSFORM);
	AddComponent(mRenderer,RENDERER);
	AddComponent(mBoundingBox,BOUNDING_BOX);
}

bool Tree::Initialize(ID3D10Device* device, HWND hwnd){
	mTreeShader = new TreeShader();
	if (!mTreeShader->Initialize(device,hwnd)){
		MessageBox(hwnd, L"Error while creating new tree shader", L"Error", MB_OK);
	}
	mTreeTexture = new TextureLoader();
	mTreeTexture->Initialize(device,L"assets/textures/tree_bark.jpg");

	return true;
}


Tree::~Tree(void){
	if (mTreeShader){
		delete mTreeShader;
		mTreeShader = nullptr;
	}
	if (mTreeTexture){
		mTreeTexture->Shutdown();
		delete mTreeTexture;
		mTreeTexture = nullptr;
	}
}


void Tree::Render(ID3DObject *d3dObject, D3DXMATRIX viewMatrix, Vector3f eyePos, Light light, int lightType){
	D3DXMATRIX mObjMatrix;
	mTransform->GetTransformMatrix(mObjMatrix);
	mObjMatrix *= *d3dObject->GetWorldMatrix();

	//calculate LOD level
	float distanceToTree = D3DXVec3Length(&(eyePos - mTransform->position));
	distanceToTree = clamp(distanceToTree,MIN_DIST,MAX_DIST);
	int lod = mapValue(distanceToTree,MIN_DIST,MAX_DIST,MAX_LOD,MIN_LOD);
	mRenderer->RenderBuffers(d3dObject->GetDevice());
	mTreeShader->Render(d3dObject->GetDevice(),mRenderer->GetIndexCount(),mObjMatrix,viewMatrix,*d3dObject->GetProjectionMatrix(),eyePos, mTreeTexture->GetTexture(), lod, light,lightType);
}

bool Tree::GenerateTreeSpaceExploration(ID3D10Device* device, float minGrowthSize, float startRadius, float maxHeight){

	const float sphereScale = maxHeight*0.5f;
	Vector3f treeShape = RandUnitVecInside3()*maxHeight;
	Vector3f sphereScaleVec = Vector3f(treeShape.x,sphereScale,treeShape.z);

	//create tree segment list needed for algorithm
	std::vector<TreeSegment*>	treeSegmentList;
	mTreeShader->SetMaxHeight(maxHeight);

	const int targetPosSize = 800;	

	float minInfluenceDist = 4.0f*minGrowthSize;
	float maxInfluenceDist = 15.0f*minGrowthSize;
	bool endGrowth = false;	//wether or not the algorithm has finished

	//generate tree trunk to half of maxHeight size
	int trunkChunks = (int)((maxHeight/2.0f)/minGrowthSize);
	
	for (int i = 0; i < trunkChunks; i++){
		TreeSegment *segment = new TreeSegment;
		segment->SetPosition(Vector3f(sphereScaleVec.x/2.0f,i*minGrowthSize,sphereScaleVec.z/2.0f));
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
		growthTargetPosition = RandUnitVecInsideScaled3(&sphereScaleVec);	
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
				parent->IncrementRadius(startRadius);				

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

	//postprocess tree here
	int minChildCountToRemain = 10;
	for (int i = 0; i < treeSegmentList.size(); i++){
		TreeSegment *segment = treeSegmentList[i];
		if (segment->GetChildCount() < minChildCountToRemain){
			treeSegmentList.erase(treeSegmentList.begin() + i);
			i--;
		}
	}
	DWORD vertexCount,indexCount;
	vertexCount = indexCount = treeSegmentList.size();
	
	TreeVertex *vertices = new TreeVertex[vertexCount];
	for (int i = 0; i < vertexCount; i++){
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

	mBoundingBox->CalculateBounds(&vertices[0].pos,vertexCount,sizeof(TreeVertex));

	DWORD	*indices = new DWORD[indexCount];
	for (int i = 0; i < indexCount; i++){
		indices[i] = i;
	}

	if (!mRenderer->InitializeBuffers(device,indices,vertices,sizeof(TreeVertex),vertexCount,indexCount)){
		return false;
	}

	//cleanup
	delete [] vertices;
	vertices = nullptr;

	delete [] indices;
	indices = nullptr;

	while (!treeSegmentList.empty()){
		TreeSegment *segment = treeSegmentList.back();
		if (segment){
			delete segment;
			segment = nullptr;
		}
		treeSegmentList.pop_back();
	}
	treeSegmentList.clear();

	return true;
}