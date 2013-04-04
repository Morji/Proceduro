#include "Water.h"


Water::Water(Vector3f position){
	mWaterShader = 0;
	mRenderer = new Renderer();
	mRenderer->SetDynamic(true);
	AddComponent(mRenderer);

	simulate = false;

	boundsMin = boundsMax = 0;

	mTransform->position = position;
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
}

bool Water::Initialize(ID3D10Device* device, HWND hwnd){
	
	//make sure the wave equation is stable
	/*if ( TIME_STEP > SPACING/(WAVE_SPEED*sqrt(2.0f)) ){
		return false;
	}*/
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
	if (!mRenderer->InitializeBuffers<WaterVertex>(device,indices,vertices,vertexCount,indexCount)){
		MessageBox(hwnd, L"Error initializing water", NULL, MB_OK);
		return false;
	}
	
	/*if (indices){
		delete [] indices;
		indices = nullptr;
	}*/

	//calculate bounding box
	boundsMin = new Vector3f();
	boundsMax = new Vector3f();
	HRESULT hr = D3DXComputeBoundingBox(&vertices[0].pos,vertexCount,sizeof(WaterVertex),boundsMin,boundsMax);
	if (FAILED(hr)){
		MessageBox(hwnd, L"Error creating water bounding box", NULL, MB_OK);
	}
	//create shader
	mWaterShader = new WaterShader();
	if (!mWaterShader->Initialize(device,hwnd)){
		MessageBox(hwnd, L"Error initializing water shader", NULL, MB_OK);
		return false;
	}

	return true;
}

void Water::Render(ID3D10Device* device,D3DXMATRIX worldMatrix,D3DXMATRIX viewMatrix,D3DXMATRIX projectionMatrix, Vector3f eyePos, Light light, int lightType){
	if (simulate){
		RunSimulation(device);
	}
	
	D3DXMATRIX mObjMatrix;
	mTransform->GetTransformMatrix(mObjMatrix);
	mObjMatrix*=worldMatrix;
	
	mRenderer->RenderBuffers(device);
	mWaterShader->SetShaderParameters(mObjMatrix,viewMatrix,projectionMatrix,eyePos,light,lightType);
	mWaterShader->Render(device,mRenderer->GetIndexCount());
}

void Water::HandlePicking(Vector3f rayOrigin, Vector3f rayDir){
	//do not do picking if not simulating
	if (!simulate){
		return;
	}

	// Transform to the mesh's local space.
	D3DXMATRIX inverseW;
	D3DXMATRIX mObjMatrix;
	mTransform->GetTransformMatrix(mObjMatrix);	
	D3DXMatrixInverse(&inverseW, 0, &mObjMatrix);
	D3DXVec3TransformCoord(&rayOrigin, &rayOrigin, &inverseW);
	D3DXVec3TransformNormal(&rayDir, &rayDir, &inverseW);

	//do not handle picking if ray outside bounding box
	bool intersectBox = D3DXBoxBoundProbe(boundsMin,boundsMax,&rayOrigin,&rayDir) != 0;

	if (!intersectBox){
		return;
	}

	//check every triangle in the mesh
	int mTriangleCount = mRenderer->GetIndexCount()/3;
	int vertexIndex = 0;
	Vector3f intersection;
	for (int i = 0; i < mTriangleCount; i++){
		CheckTriangleIntersection(i,rayOrigin,rayDir,intersection);
	}
}

bool Water::CheckTriangleIntersection(int index,Vector3f &rayOrigin,Vector3f &rayDir, Vector3f &intersectionPoint){
	int vertexIndex = 0;
	vertexIndex = index * 3;

 	int i = vertexIndex/(GRIDSIZE-1);
	int j = vertexIndex%(GRIDSIZE-1);

	Vector3f *v0,*v1,*v2;
	// Get the three vertices of this triangle from the vertex list.
	v0 = &vertices[indices[vertexIndex]].pos;
	v1 = &vertices[indices[vertexIndex+1]].pos;
	v2 = &vertices[indices[vertexIndex+2]].pos;	

	float u,v,dist;

	bool result = D3DXIntersectTri(v0,v1,v2,&rayOrigin,&rayDir,&u,&v,&dist) != 0;

	if (result){
		v0->y += cos(dist)/1.5f;
		v1->y += cos(dist)/1.5f;
		v2->y += cos(dist)/1.5f;
	}
	
	return result;
}

void Water::RunSimulation(ID3D10Device* device){

	for(int i = 0; i < GRIDSIZE; ++i){
		for(int j = 0; j < GRIDSIZE; ++j){
			ComputeWaveEqn(i,j);
		}
	}
	
	WaterVertex *newVertices = 0;
	mRenderer->GetVertexBuffer()->Map(D3D10_MAP_WRITE_DISCARD, 0, (void**)&newVertices);

	for(int i = 0; i < GRIDSIZE; ++i){
		for(int j = 0; j < GRIDSIZE; ++j){
			int index = i*GRIDSIZE+j;
			prevStepHeight[index] = vertices[index].pos.y;
			vertices[index].pos.y = nextStepHeight[index];	
			newVertices[index] = vertices[index];
		}
	}
	mRenderer->GetVertexBuffer()->Unmap();
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

	float s1Norm = (height - uPrev)/2.0f*CELLSPACING;
	Vector3f T = Vector3f(0,s1Norm,1);
	Vector3f B = Vector3f(1,s1Norm,0);

	Vector3f normal;
	D3DXVec3Cross(&normal,&T,&B);
	D3DXVec3Normalize(&normal,&normal);
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
	simulate = state;
}

bool Water::GetSimulationState(){
	return simulate;
}