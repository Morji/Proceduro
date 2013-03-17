#include "Terrain.h"

Terrain::Terrain(void)
{
	maxHeight = 0.0f;
	terrainGeneratedToggle = false;
	heightData = nullptr;
	vertices = nullptr;
	indices = nullptr;
	texOffset = Vector2f(0,0);
	animCoeff = 0.0f;
	mTerrainShader = 0;
	specularMap = 0;
	mVB = 0;
	mIB = 0;
	offset = stride = 0;
	for (int i = 0; i < 3; i++)
		diffuseMapRV[i] = 0;


}


Terrain::~Terrain(void)
{
	if (vertices){
		delete [] vertices;
		vertices = nullptr;		
	}
	if (indices){
		delete [] indices;
		indices = nullptr;		
	}
	if (heightData){
		delete [] heightData;
		heightData = nullptr;
	}

	if (mTerrainShader){
		delete mTerrainShader;
		mTerrainShader = nullptr;
	}

	if (specularMap){
		specularMap->Shutdown();
		delete specularMap;
		specularMap = nullptr;
	}
	for (int i = 0; i < 3; i++){
		if (diffuseMapRV[i]){
			diffuseMapRV[i]->Shutdown();
			delete diffuseMapRV[i];
			diffuseMapRV[i] = nullptr;
		}
	}
	md3dDevice = nullptr;
	// Release the index buffer.
	ReleaseCOM(mIB);
	// Release the vertex buffer.
	ReleaseCOM(mVB);
}

bool Terrain::Initialize(ID3D10Device* device, HWND hwnd){
	md3dDevice = device;

	mTerrainShader = new TerrainHeightShader();
	if (!mTerrainShader->Initialize(md3dDevice,hwnd)){
		MessageBox(hwnd, L"Error while creating terrain shader", L"Error", MB_OK);
		return false;
	}
	specularMap = new TextureLoader();
	if (!specularMap->Initialize(md3dDevice,L"assets/textures/defaultspec.dds")){
		MessageBox(hwnd, L"Error while creating terrain textures", L"Error", MB_OK);
		return false;
	}
	for (int i = 0; i < 3; i++){
		diffuseMapRV[i] = new TextureLoader();
		WCHAR *texFile = 0;
		if (i == 0)
			texFile = L"assets/textures/stone2.dds";
		else if (i == 1)
			texFile = L"assets/textures/ground0.dds";
		else if (i == 2)
			texFile = L"assets/textures/grass0.dds";
		if (!diffuseMapRV[i]->Initialize(md3dDevice,texFile)){
			MessageBox(hwnd, L"Error while creating terrain textures", L"Error", MB_OK);
			return false;
		}
		texFile = 0;
	}
	return true;
}

void Terrain::RenderBuffers(){
	// Set vertex buffer stride and offset.
	offset = 0;
	// Set the vertex buffer to active in the input assembler so it can be rendered.
	md3dDevice->IASetVertexBuffers(0, 1, &mVB, &stride, &offset);

	// Set the index buffer to active in the input assembler so it can be rendered.
	md3dDevice->IASetIndexBuffer(mIB, DXGI_FORMAT_R32_UINT, 0);

	// Set the type of primitive that should be rendered from this vertex buffer, in this case triangles.
	md3dDevice->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void Terrain::Render(D3DXMATRIX worldMatrix,D3DXMATRIX viewMatrix,D3DXMATRIX projectionMatrix, Vector3f eyePos, Light light, int lightType){
	D3DXMATRIX mObjMatrix;
	mTransform->GetTransformMatrix(mObjMatrix);
	mObjMatrix*=worldMatrix;
	RenderBuffers();
	mTerrainShader->Render(md3dDevice,mIndexCount,mObjMatrix,viewMatrix,projectionMatrix,
							eyePos,
							light,
							diffuseMapRV[0]->GetTexture(),
							diffuseMapRV[1]->GetTexture(),
							diffuseMapRV[2]->GetTexture(),
							maxHeight,
							lightType);
}

bool Terrain::CreateTerrain(TerrainGenerator *generator){
	if (!generator){
		return false;
	}
	ResetData();
	gridWidth = generator->GetWidth();
	gridDepth = generator->GetDepth();

	mVertexCount = (gridWidth*gridDepth);
	vertices = new VertexNT[mVertexCount];

	heightData = new float[gridWidth*gridDepth];

	float dx = CELLSPACING;
	float halfWidth = (gridWidth-1)*dx*0.5f;
	float halfDepth = (gridDepth-1)*dx*0.5f;	

	int index = 0;

	for(int i = 0; i < gridWidth; ++i){
		float z = halfDepth - i*dx;		
		for(int j = 0; j < gridDepth; ++j){
			float x = -halfWidth + j*dx;
			float y = generator->GetHeight(i,j);
			index = i*gridWidth+j;
			vertices[index].pos = D3DXVECTOR3(x, y, z);
			heightData[index] = y;
		}
	}

	maxHeight = generator->GetMaxHeight();
	
	ComputeTextureCoords();
	ComputeNormals();

	ComputeIndices();

	//initialize the buffers with the index and vertex data
	if (!InitializeBuffers(indices, vertices))
		return false;

	delete [] vertices;
	vertices = nullptr;
	delete [] indices;
	indices = nullptr;

	return true;
}

void Terrain::ResetData(){
	maxHeight = 0.0f;
	// Release the index buffer.
	ReleaseCOM(mIB);
	// Release the vertex buffer.
	ReleaseCOM(mVB);
	if (vertices){
		delete [] vertices;
		vertices = nullptr;
	}
	if (indices){
		delete [] indices;
		indices = nullptr;
	}
	if (heightData){
		delete [] heightData;
		heightData = nullptr;
	}
}

void Terrain::AnimateTerrain(float dt){
	animCoeff += dt;
	for(int i=0; i<gridWidth; i++){
		for(int j=0; j<gridDepth; j++){		
			int index = (gridWidth * i) + j;				
			float height = (float)(sin((float)j/(gridWidth/animCoeff))*10.0f); //magic numbers ahoy, just to ramp up the height of the sin function so its visible.
			vertices[index].pos.y = height;
			heightData[index] = height;//place in height data array
			if (height > maxHeight){
				maxHeight = height;
			}
		}
	}
	// Release the index buffer.
	ReleaseCOM(mIB);
	// Release the vertex buffer.
	ReleaseCOM(mVB);
	if (!InitializeBuffers(indices, vertices))
		return ;
}

void Terrain::ComputeIndices(){
	// Iterate over each quad and compute indices.
	mIndexCount = ((gridWidth-1)*(gridDepth-1)*6);
	indices = new DWORD[mIndexCount];
	int k = 0;
	bool switchPtrn = false;
	for(int i = 0; i < gridWidth-1; ++i){
		for(int j = 0; j < gridDepth-1; ++j){

			if (switchPtrn == false){
				indices[k] = i*gridDepth+j;
				indices[k+1] = i*gridDepth+j+1;
				indices[k+2] = (i+1)*gridDepth+j;

				indices[k+3] = (i+1)*gridDepth+j;
				indices[k+4] = i*gridDepth+j+1;
				indices[k+5] = (i+1)*gridDepth+j+1;
			}
			else{
				indices[k] = i*gridDepth+j;
				indices[k+1] = i*gridDepth+j+1;
				indices[k+2] = (i+1)*gridDepth+j+1;

				indices[k+3] = (i+1)*gridDepth+j+1;
				indices[k+4] = (i+1)*gridDepth+j;
				indices[k+5] = i*gridDepth+j;
			}
			k += 6; // next quad
			switchPtrn = !switchPtrn;
		}
		switchPtrn = (gridWidth % 2 == 1) ?  !switchPtrn : switchPtrn;
	}
}

//compute the vertex normals
void Terrain::ComputeNormals()const{

	for (int i = 0; i < gridWidth; i++){
		for (int j = 0; j < gridDepth; j++){
			Vector3f v1,v2,v3,v4,v12,v23,v34,v41,v;
			v1 = v2 = v3 = v4 = v12 = v23 = v34 = v41 = v = Vector3f(0.0f,0.0f,0.0f);
			//grab 2 vectors for this
			if (j != gridWidth - 1){
				v1 = Vector3f(vertices[i*gridDepth+j+1].pos-vertices[i*gridDepth+j].pos);
			}
			if (i != gridWidth - 1){
				v2 = Vector3f(vertices[(i+1)*gridDepth+j].pos-vertices[i*gridDepth+j].pos);
			}
			if (j > 0){
				v3 = Vector3f(vertices[i*gridDepth+j-1].pos-vertices[i*gridDepth+j].pos);
			}
			if (i > 0){
				v4 = Vector3f(vertices[(i-1)*gridDepth+j].pos-vertices[i*gridDepth+j].pos);
			}

			D3DXVec3Cross(&v12,&v1,&v2);
			D3DXVec3Normalize(&v12,&v12);

			D3DXVec3Cross(&v23,&v2,&v3);
			D3DXVec3Normalize(&v23,&v23);
			
			D3DXVec3Cross(&v34,&v3,&v4);
			D3DXVec3Normalize(&v34,&v34);
			
			D3DXVec3Cross(&v41,&v4,&v1);
			D3DXVec3Normalize(&v41,&v41);

			
			if (D3DXVec3Length(&v12) > 0.0f)
				v = v + v12;

			if (D3DXVec3Length(&v23) > 0.0f)
				v = v + v23;
			
			if (D3DXVec3Length(&v34) > 0.0f)
				v = v + v34;

			if (D3DXVec3Length(&v41) > 0.0f)
				v = v + v41;

			D3DXVec3Normalize(&v,&v);

			vertices[i*gridDepth+j].normal = v;			
		}
	}
}

void Terrain::ComputeTextureCoords(const int repeatAmount)const{

	float incrementValue;

	// Calculate how much to increment the texture coordinates by.
	incrementValue = (float)repeatAmount / (float)gridDepth;

	float widthRepeat = (float)(gridWidth/repeatAmount);
	float depthRepeat = (float)(gridDepth/repeatAmount);

	// Loop through the entire height map and calculate the tu and tv texture coordinates for each vertex.
	for(int i=0; i<gridWidth; i++){
		for(int j=0; j<gridDepth; j++){
			vertices[i*gridDepth+j].texC = Vector2f(i / widthRepeat, j / depthRepeat);
		}
	}
}

//The InitializeBuffers function is where we handle creating the vertex and index buffers. 
bool Terrain::InitializeBuffers(DWORD* indices,  VertexNT* vertices){

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
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * mIndexCount;
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
	stride = sizeof(VertexNT);
	return true;	
}

void Terrain::AnimateUV(float dt){
	// Animate water texture as a function of time in the update function.
	texOffset.y += 0.1f*dt;
	texOffset.x = 0.25f*sinf(4.0f*texOffset.y);

	// Scale texture coordinates by 5 units to map [0,1]-->[0,5]
	// so that the texture repeats five times in each direction.
	D3DXMATRIX S;
	D3DXMatrixScaling(&S, 5.0f, 5.0f, 1.0f);
	// Translate the texture.
	D3DXMATRIX T;
	D3DXMatrixTranslation(&T, texOffset.x, texOffset.y, 0.0f);
	// Scale and translate the texture.
	//mTexMatrix = S*T;
}

float Terrain::GetMaxHeight(){
	return maxHeight;
}

float Terrain::GetHeight(float x, float z){
	// Transform from terrain local space to “cell” space.
	float c = (x + 0.5f*gridWidth) / CELLSPACING;
	float d = (z - 0.5f*gridDepth) / -CELLSPACING;

	//check if point is within the grid
	if (c < 0 || c > gridWidth || d < 0 || d > gridDepth){
		return 0.0f;
	}

	// Get the row and column we are in.
	int row = (int)floorf(d);//z
	int col = (int)floorf(c);//x

	// Grab the heights of the cell we are in.
	// A*--*B
	// | /|
	// |/ |
	// C*--*D
	float A = heightData[row*gridWidth + col];
	float B = heightData[row*gridWidth + col + 1];
	float C = heightData[(row+1)*gridWidth + col];
	float D = heightData[(row+1)*gridWidth + col + 1];

	// Find out which of the two triangles of the cell we are in
	// if s + t <= 1 we are in the upper triangle
	// else we are in the lower one
	float s = c - (float)col;
	float t = d - (float)row;

	// If upper triangle ABC.
	if( s + t <= 1.0f){
		float uy = B - A;
		float vy = C - A;
		return A + s*uy + t*vy;
	}
	// lower triangle DCB.
	else{ 	
		float uy = C - D;
		float vy = B - D;
		return D + (1.0f-s)*uy + (1.0f-t)*vy;
	}
}

Vector3f Terrain::GetRandomPoint(){
	float dx = CELLSPACING;
	float halfWidth = (gridWidth-1)*dx*0.5f;
	float halfDepth = (gridDepth-1)*dx*0.5f;

	float z = halfDepth - RandF(0,gridWidth)*dx;		
	float x = -halfWidth + RandF(0,gridDepth)*dx;

	float y = GetHeight(x,z);	

	return Vector3f(x,y,z);
}