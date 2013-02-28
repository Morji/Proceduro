#include "Tree.h"


Tree::Tree(void){
	md3dDevice = 0;
	treeShader = 0;
	mTreePosition = Vector3f(0,0,0);
	treeTexture = 0;
}

Tree::Tree(ID3D10Device *d3dDevice, HWND hwnd, Vector3f position){
	md3dDevice = d3dDevice;
	mTreePosition = position;
	treeShader = new TexShader();
	if (!treeShader->Initialize(md3dDevice,hwnd)){
		MessageBox(hwnd, L"Error while creating tree shader", L"Error", MB_OK);
	}

	treeTexture = new TextureLoader();
	treeTexture->Initialize(md3dDevice,L"assets/textures/tree_texture.jpg");
}


Tree::~Tree(void){
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
	if (treeTexture){
		treeTexture->Shutdown();
		delete treeTexture;
		treeTexture = nullptr;
	}
	md3dDevice = 0;
}

void Tree::Render(D3DXMATRIX worldMatrix,D3DXMATRIX viewMatrix,D3DXMATRIX projectionMatrix, Vector3f eyePos, Light light, int lightType){
	D3DXMATRIX m,mObjMatrix;
	D3DXMatrixIdentity(&mObjMatrix);
	D3DXMatrixTranslation(&m, mTreePosition.x, mTreePosition.y, mTreePosition.z);
	mObjMatrix *= m;
	mObjMatrix *= worldMatrix;

	for (int i = 0; i < treeSegmentList.size(); i++){
		treeSegmentList[i]->GetGameObject()->Render(mObjMatrix);
		treeShader->Render(md3dDevice,treeSegmentList[i]->GetGameObject()->GetIndexCount(),treeSegmentList[i]->GetGameObject()->objMatrix,viewMatrix,projectionMatrix,treeSegmentList[i]->GetGameObject()->GetTexMatrix(),eyePos,light,lightType,treeTexture->GetTexture(),NULL);
	}

}

bool Tree::GenerateTreeSpaceExploration(float minGrowthSize, float startRadius, float minInfluenceDist, float maxInfluenceDist, float maxHeight){

	const int targetPosSize = 700;
	const float sphereScale = maxHeight*0.5f;

	bool endGrowth = false;	//wether or not the algorithm has finished

	//generate tree trunk to half of maxHeight size
	int trunkChunks = (int)((maxHeight*0.4f)/minGrowthSize);
	
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

		/*TreeSegment *segment = new TreeSegment;
		segment->SetPosition(growthTargetPosition);
		segment->SetHeight(0.1f);
		segment->SetRadius(0.05f);
		treeSegmentList.push_back(segment);*/	
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

				//get angle of new segment
				Vector3f upVector = Vector3f(0,1,0);
				float angle2F = acos(D3DXVec3Dot(&upVector,&growthDirection));
				Vector3f rotAxisVec;
				D3DXVec3Cross(&rotAxisVec,&upVector,&growthDirection);
				D3DXVec3Normalize(&rotAxisVec,&rotAxisVec);				
				Vector3f quatXYZ = rotAxisVec*sin(angle2F/2.0f);
				float quatW = cos(angle2F/2.0f);
				D3DXQUATERNION quaternionRot = D3DXQUATERNION(quatXYZ.x,quatXYZ.y,quatXYZ.z,quatW);

				D3DXQuaternionNormalize(&quaternionRot,&quaternionRot);
				

				TreeSegment *newSegment = new TreeSegment;
				newSegment->SetPosition(newSegmentPosition);
				newSegment->SetHeight(minGrowthSize);
				newSegment->SetRadius(startRadius*2);
				newSegment->SetParent(treeSegment);
				newSegment->GetGameObject()->SetRotation(&quaternionRot);
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

	//Initialize all tree segments
	for (int i = 0; i < treeSegmentList.size(); i++){
		TreeSegment *segment = treeSegmentList[i];
		if (!segment->GetGameObject()->Initialize(md3dDevice)){
			return false;
		}
	}

	return true;
}
