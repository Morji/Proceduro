#include "Water.h"


Water::Water(Vector3f position){
	mWaterShader = nullptr;
	simulateWave = simulateHeat = false;

	mRenderer = new Renderer(this);
	mRenderer->SetDynamic(true);
	AddComponent(mRenderer,RENDERER);
	
	mBoundingBox = new BoundingBox(this);
	AddComponent(mBoundingBox,BOUNDING_BOX);

	mTransform = new Transform(this);
	mTransform->position = position;
	AddComponent(mTransform,TRANSFORM);
}


Water::~Water(void){
	if (mWaterShader){
		delete mWaterShader;
		mWaterShader = nullptr;
	}
	if (indices){
		delete [] indices;
		indices = nullptr;
	}
	if (vertices){
		delete [] vertices;
		vertices = nullptr;
	}
	if (prevStepHeight){
		delete [] prevStepHeight;
		prevStepHeight = nullptr;
	}
	if (nextStepHeight){
		delete [] nextStepHeight;
		nextStepHeight = nullptr;
	}
	if (currHeat){
		delete [] prevStepHeight;
		prevStepHeight = nullptr;
	}
	if (nextStepHeat){
		delete [] nextStepHeight;
		nextStepHeight = nullptr;
	}
}

bool Water::Initialize(ID3D10Device* device, HWND hwnd){
	
	//make sure the wave equation is stable
	float stableNumber = DAMP_FACTOR + sqrt(32*WAVE_SPEED*WAVE_SPEED/SPACING*SPACING)/(8*WAVE_SPEED*WAVE_SPEED/SPACING*SPACING);
	if ( TIME_STEP > stableNumber ){
		return false;
	}	
	
	float dx = CELLSPACING;
	float halfWidth = (GRIDSIZE-1)*dx*0.5f;
	float halfDepth = (GRIDSIZE-1)*dx*0.5f;	

	int index = 0;
	int vertexCount = GRIDSIZE*GRIDSIZE;
	int triangleCount = vertexCount / 3;
	prevStepHeight = new float[vertexCount];
	nextStepHeight = new float[vertexCount];

	currHeat = new float[vertexCount];
	nextStepHeat = new float[vertexCount];

	vertices = new WaterVertex[vertexCount];
	for(int i = 0; i < GRIDSIZE; ++i){
		float z = halfDepth - i*dx;		
		for(int j = 0; j < GRIDSIZE; ++j){
			float x = -halfWidth + j*dx;
			float y = 0.0f;
			index = i*GRIDSIZE+j;
			prevStepHeight[index] = 0;
			vertices[index].pos = Vector3f(x, y, z);
			vertices[index].normal = Vector3f(0,1,0);
			currHeat[index] = 0.0f;
			vertices[index].color = WATER_BLUE_COLOR;
			vertices[index].color.r = currHeat[index];
			vertices[index].color.b = 1.0f - currHeat[index];
		}
	}

	int indexCount = (GRIDSIZE-1)*(GRIDSIZE-1)*6;
	indices = new DWORD[indexCount];	

	int k = 0;
	bool switchPtrn = false;
	for(int i = 0; i < GRIDSIZE-1; ++i){
		for(int j = 0; j < GRIDSIZE-1; ++j){
			if (switchPtrn == false){
				indices[k] = i*GRIDSIZE+j;
				indices[k+1] = i*GRIDSIZE+j+1;
				indices[k+2] = (i+1)*GRIDSIZE+j;

				indices[k+3] = (i+1)*GRIDSIZE+j;
				indices[k+4] = i*GRIDSIZE+j+1;
				indices[k+5] = (i+1)*GRIDSIZE+j+1;
			}
			else{
				indices[k] = i*GRIDSIZE+j;
				indices[k+1] = i*GRIDSIZE+j+1;
				indices[k+2] = (i+1)*GRIDSIZE+j+1;

				indices[k+3] = (i+1)*GRIDSIZE+j+1;
				indices[k+4] = (i+1)*GRIDSIZE+j;
				indices[k+5] = i*GRIDSIZE+j;
			}
			k += 6; // next quad
			switchPtrn = !switchPtrn;
		}
		switchPtrn = (GRIDSIZE % 2 == 1) ?  !switchPtrn : switchPtrn;
	}
	int iter = 0;
	for(int i = GRIDSIZE/2 - GRIDSIZE/8; i < GRIDSIZE/2 + GRIDSIZE/8; ++i){
		for(int j = GRIDSIZE/2 - GRIDSIZE/8; j < GRIDSIZE/2 + GRIDSIZE/8; ++j){
			index = i*GRIDSIZE+j;
			iter++;
			float height = (float)(sin((float)(iter/2)/(GRIDSIZE))*2.0f);
			prevStepHeight[index] = height;
			vertices[index].pos.y = height;
		}
	}
	if (!mRenderer->InitializeBuffers(device,indices,vertices,sizeof(WaterVertex),vertexCount,indexCount)){
		MessageBox(hwnd, L"Error initializing water", NULL, MB_OK);
		return false;
	}

	//calculate bounding box
	bool result = mBoundingBox->CalculateBounds(&vertices[0].pos,vertexCount,sizeof(WaterVertex));
	if (!result){
		MessageBox(hwnd, L"Error initializing water bounding box", NULL, MB_OK);
		return false;
	}
	//create shader
	mWaterShader = new WaterShader();
	if (!mWaterShader->Initialize(device,hwnd)){
		MessageBox(hwnd, L"Error initializing water shader", NULL, MB_OK);
		return false;
	}

	return true;
}

void Water::Render(ID3DObject* d3dObject, D3DXMATRIX viewMatrix, Vector3f eyePos, Light light, int lightType){
	if (simulateWave || simulateHeat){
		RunSimulation(d3dObject->GetDevice());
	}
	
	D3DXMATRIX mObjMatrix;
	d3dObject->GetWorldMatrix(mObjMatrix);
	mTransform->GetTransformMatrix(mObjMatrix);
	
	mRenderer->RenderBuffers(d3dObject->GetDevice());
	mWaterShader->SetShaderParameters(mObjMatrix,viewMatrix,*d3dObject->GetProjectionMatrix(),eyePos,light,lightType);
	mWaterShader->Render(d3dObject->GetDevice(),mRenderer->GetIndexCount());
}

bool Water::CanHandlePicking(Vector3f rayOrigin, Vector3f rayDir){
	//do not do picking if not simulating
	if (!simulateWave && !simulateHeat){
		return false;
	}
	Vector3f localRayOrigin;
	Vector3f localRayDir;
	if (!mBoundingBox->IntersectsRay(rayOrigin,rayDir,&localRayOrigin,&localRayDir)){
		return false;
	}

	//check every triangle in the mesh
	int mTriangleCount = mRenderer->GetIndexCount()/3;
	int vertexIndex = 0;
	Vector3f intersection;
	for (int i = 0; i < mTriangleCount; i++){
		CheckTriangleIntersection(i,&localRayOrigin,&localRayDir,&intersection);
	}
	return true; // water handled picking
}

bool Water::CheckTriangleIntersection(int index,Vector3f *rayOrigin,Vector3f *rayDir, Vector3f *outIntersectionPoint){
	int vertexIndex = 0;
	vertexIndex = index * 3;

	Vector3f *v0,*v1,*v2;
	// Get the three vertices of this triangle from the vertex list.
	v0 = &vertices[indices[vertexIndex]].pos;
	v1 = &vertices[indices[vertexIndex+1]].pos;
	v2 = &vertices[indices[vertexIndex+2]].pos;	

	float u,v,dist;

	bool result = D3DXIntersectTri(v0,v1,v2,rayOrigin,rayDir,&u,&v,&dist) != 0;

	if (result){
		//holding shift modifies heat, else modify waves
		if (GetAsyncKeyState(VK_SHIFT)){
			//grab 3 triangles
			if (vertexIndex > 3){
				currHeat[indices[vertexIndex]] = 1.0f;
				currHeat[indices[vertexIndex-1]] = 1.0f;
				currHeat[indices[vertexIndex-2]] = 1.0f;
			}
			currHeat[indices[vertexIndex]] = 1.0f;
			currHeat[indices[vertexIndex+1]] = 1.0f;
			currHeat[indices[vertexIndex+2]] = 1.0f;
			if (vertexIndex+3 < mRenderer->GetIndexCount()){
				currHeat[indices[vertexIndex+2]] = 1.0f;
				currHeat[indices[vertexIndex+3]] = 1.0f;
				currHeat[indices[vertexIndex+4]] = 1.0f;
			}
		}
		else{
			float amount = cos(dist)/1.5f;
			v0->y += amount;
			v1->y += amount;
			v2->y += amount;
			if (vertexIndex > 3){
				vertices[indices[vertexIndex-2]].pos.y += amount;
				vertices[indices[vertexIndex-1]].pos.y += amount;
				vertices[indices[vertexIndex]].pos.y += amount;
			}
			if (vertexIndex+3 < mRenderer->GetIndexCount()){
				vertices[indices[vertexIndex+2]].pos.y += amount;
				vertices[indices[vertexIndex+3]].pos.y += amount;
				vertices[indices[vertexIndex+4]].pos.y += amount;
			}	
		}
	}
	
	return result;
}

void Water::RunSimulation(ID3D10Device* device){

	for(int i = 0; i < GRIDSIZE; ++i){
		for(int j = 0; j < GRIDSIZE; ++j){
			ComputeWaveEqn(i,j);
			ComputeHeatEqn(i,j);
		}
	}
	
	WaterVertex *newVertices = 0;
	mRenderer->GetVertexBuffer()->Map(D3D10_MAP_WRITE_DISCARD, 0, (void**)&newVertices);

	for(int i = 0; i < GRIDSIZE; ++i){
		for(int j = 0; j < GRIDSIZE; ++j){
			int index = i*GRIDSIZE+j;
			//wave equation results
			prevStepHeight[index] = vertices[index].pos.y;			
			vertices[index].pos.y = nextStepHeight[index];				
			newVertices[index] = vertices[index];

			//heat equation results
			currHeat[index] = nextStepHeat[index];
			vertices[index].color.r = currHeat[index];
			vertices[index].color.b = 1.0f - currHeat[index];
		}
	}
	mRenderer->GetVertexBuffer()->Unmap();
}

void Water::ComputeHeatEqn(int i, int j){
	int index = i*GRIDSIZE+j;

	if (i <= 0 || j <= 0 || i >= GRIDSIZE-1 || j >= GRIDSIZE-1){
		nextStepHeat[index] = 0.0f;
		return;
	}

	//compute heat equation using euler method
	if (COURANT_NUMBER > 0.5f){
		return;
	}
	float heat = currHeat[index] + COURANT_NUMBER*( GetHeat(i-1,j) + GetHeat(i+1,j) + GetHeat(i,j-1) + GetHeat(i,j+1) - 4*GetHeat(i,j) );
	nextStepHeat[index] = heat;
}

float Water::GetHeat( int i, int j){	
	if (i <= 0 || j <= 0 || i >= GRIDSIZE-1 || j >= GRIDSIZE-1){
		return 0.0f;
	}
	int index = i*GRIDSIZE+j;
	return currHeat[index];
}

void Water::ComputeWaveEqn(int i, int j){
	int index = i*GRIDSIZE+j;

	if (i <= 0 || j <= 0 || i >= GRIDSIZE-1 || j >= GRIDSIZE-1){
		nextStepHeight[index] = 0.0f;
		return;
	}

	float u = GetHeight(i,j);	
	float uPrev = prevStepHeight[index];
	float ctx2 = WAVE_SPEED*TIME_STEP/SPACING;
	ctx2 *= ctx2;
	float height = 2*u - uPrev*( 1-(TIME_STEP/DAMP_FACTOR/2.0f) ) +  (GetHeight(i-1,j) + GetHeight(i+1,j) + GetHeight(i,j-1) + GetHeight(i,j+1) - 4*u)*ctx2;
	height /= 1+(TIME_STEP/DAMP_FACTOR/2.0f);

	Vector3f normal = Vector3f( (GetHeight(i+1,j) - GetHeight(i-1,j))/2*CELLSPACING,1,-(GetHeight(i,j+1) - GetHeight(i,j-1))/2*CELLSPACING );
	vertices[index].normal = normal;

	nextStepHeight[index] = height;
}

float Water::GetHeight( int i, int j){	

	if (i <= 0 || j <= 0 || i >= GRIDSIZE-1 || j >= GRIDSIZE-1){
		return 0.0f;
	}
	int index = i*GRIDSIZE+j;
	return vertices[index].pos.y;
}

void Water::SetSimulationState(bool state){
	simulateHeat = state;
	simulateWave = state;
}

bool Water::GetSimulationState(){
	return simulateHeat && simulateWave;
}