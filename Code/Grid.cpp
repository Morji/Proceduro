#include "Grid.h"

Grid::Grid(void)
{
	maxHeight = 0.0f;
	terrainGeneratedToggle = false;
	heightData = nullptr;
	vertices = nullptr;
	indices = nullptr;
	texOffset = Vector2f(0,0);
	animCoeff = 0.0f;
}


Grid::~Grid(void)
{
	Shutdown();
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

bool Grid::GenerateGridFromTGA(char* filename){
	TerrainLoader *terrainLoader = new TerrainLoader();

	if (!terrainLoader->LoadTerrain(filename)){
		return false;
	}

	gridWidth = terrainLoader->GetWidth();
	gridDepth = terrainLoader->GetDepth();

	mVertexCount = (gridWidth*gridDepth);
	vertices = new VertexNT[mVertexCount];

	heightData = new float[gridWidth*gridDepth];

	float dx = CELLSPACING;
	float halfWidth = (gridWidth-1)*dx*0.5f;
	float halfDepth = (gridDepth-1)*dx*0.5f;

	for(int i = 0; i < gridWidth; ++i){
		float z = halfDepth - i*dx;

		for(int j = 0; j < gridDepth; ++j){
			float x = -halfWidth + j*dx;
			// Graph of this function looks like a mountain range.
			float y = terrainLoader->GetHeight(i,j)*HEIGHT_FACTOR;
			heightData[i*gridDepth+j] = y;//place in height data array
			vertices[i*gridDepth+j].pos = Vector3f(x, y, z);
			if (y > maxHeight){
				maxHeight = y;
			}
		}
	}
	ComputeNormals();
	ComputeTextureCoords();
	
	// Iterate over each quad and compute indices.
	mIndexCount = ((gridWidth-1)*(gridDepth-1)*6);
	indices = new DWORD[mIndexCount];
	int k = 0;
	for(int i = 0; i < gridWidth-1; ++i){
		for(int j = 0; j < gridDepth-1; ++j){
			// Upper left.

			indices[k] = i*gridDepth+j;
			indices[k+1] = i*gridDepth+j+1;
			indices[k+2] = (i+1)*gridDepth+j;

			indices[k+3] = (i+1)*gridDepth+j;
			indices[k+4] = i*gridDepth+j+1;
			indices[k+5] = (i+1)*gridDepth+j+1;
			k += 6; // next quad
		}
	}

	//initialize the buffers with the index and vertex data
	if (!InitializeBuffers(indices, vertices))
		return false;

	delete terrainLoader;
	terrainLoader = nullptr;

	return true;
}

///Generate a a flat grid given
bool Grid::GenerateGrid(int width, int depth){

	gridWidth = width;
	gridDepth = depth;

	mVertexCount = (gridWidth*gridDepth);
	vertices = new VertexNT[mVertexCount];

	heightData = new float[gridWidth*gridDepth];

	float dx = CELLSPACING;
	float halfWidth = (width-1)*dx*0.5f;
	float halfDepth = (depth-1)*dx*0.5f;	

	for(DWORD i = 0; i < width; ++i){

		float z = halfDepth - i*dx;
		
		for(DWORD j = 0; j < depth; ++j){
			float x = -halfWidth + j*dx;
			float y = 0.0f;
			vertices[i*depth+j].pos = D3DXVECTOR3(x, y, z);
			vertices[i*depth+j].normal = D3DXVECTOR3(0,1,0);
		}
	}
	
	ComputeTextureCoords();

	ComputeIndices();

	//initialize the buffers with the index and vertex data
	if (!InitializeBuffers(indices, vertices))
		return false;

	return true;
}

bool Grid::NoiseOverTerrain(float maxHeightToAdd, float smoothnessFactor){
	//make sure there is terrain to noise over
	if (gridWidth < 1 || gridDepth < 1)
		return false;

	maxHeight = 0.0f;

	SimplexNoise::Init(rand());

	for(DWORD i = 0; i < gridWidth; ++i){
		for(DWORD j = 0; j < gridDepth; ++j){
			int index = (gridWidth * i) + j;	
			double noise = SimplexNoise::Noise2D(i/smoothnessFactor,j/smoothnessFactor);
			float height = vertices[index].pos.y + (noise*maxHeightToAdd);
			vertices[index].pos.y = height;
			heightData[index] = height;//place in height data array
			if (height > maxHeight){
				maxHeight = height;
			}
		}
	}
	ComputeNormals();
	// Release the index buffer.
	ReleaseCOM(mIB);
	// Release the vertex buffer.
	ReleaseCOM(mVB);
	if (!InitializeBuffers(indices, vertices))
		return false;

	return true;
}

void Grid::ResetData(){
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

bool Grid::GenerateHeightMap(bool keydown, int width, int depth){

	//the toggle is just a bool that I use to make sure this is only called ONCE when you press a key
	//until you release the key and start again. We dont want to be generating the terrain 500
	//times per second. 
	if(keydown&&(!terrainGeneratedToggle)){
		ResetData();
		int index;
		float height = 0.0;
		
		gridWidth = width;
		gridDepth = depth;

		mVertexCount = (gridWidth*gridDepth);
		vertices = new VertexNT[mVertexCount];

		heightData = new float[gridWidth*gridDepth];

		float dx = CELLSPACING;
		float halfWidth = (gridWidth-1)*dx*0.5f;
		float halfDepth = (gridDepth-1)*dx*0.5f;
		//loop through the terrain and set the hieghts how we want. This is where we generate the terrain
		//in this case I will run a sin-wave through the terrain in one axis.

 		for(int i=0; i<gridWidth; i++){
			float z = halfDepth - i*dx;

			for(int j=0; j<gridDepth; j++){		
				float x = -halfWidth + j*dx;

				index = (gridWidth * i) + j;				
				float height = (float)(sin((float)j/(gridWidth/30))*10.0); //magic numbers ahoy, just to ramp up the height of the sin function so its visible.
				vertices[index].pos = D3DXVECTOR3(x,height,z);
				heightData[index] = height;//place in height data array
				if (height > maxHeight){
					maxHeight = height;
				}
			}
		}

		ComputeNormals();
		ComputeTextureCoords();

		ComputeIndices();

	//initialize the buffers with the index and vertex data
	if (!InitializeBuffers(indices, vertices))
		return false;

		terrainGeneratedToggle = true;
	}
	else{
		terrainGeneratedToggle = false;
	}
	return true;
}

bool Grid::GenerateGridFaultLine(int iterations, float displacementFactor, int width, int depth){
	ResetData();
	int index;
	float height = 0.0f;
	
	gridWidth = width;
	gridDepth = depth;

	mVertexCount = (gridWidth*gridDepth);
	vertices = new VertexNT[mVertexCount];

	heightData = new float[gridWidth*gridDepth];

	float dx = CELLSPACING;
	float halfWidth = (gridWidth-1)*dx*0.5f;
	float halfDepth = (gridDepth-1)*dx*0.5f;
	//loop through the terrain and set the hieghts how we want. This is where we generate the terrain
	//in this case I will run a sin-wave through the terrain in one axis.
	float d = sqrt((float)gridWidth*gridWidth + (float)gridDepth*gridDepth);

 	for(int i=0; i<gridWidth; i++){
		float z = halfDepth - i*dx;

		for(int j=0; j<gridDepth; j++){		
			float x = -halfWidth + j*dx;
			index = (gridWidth * i) + j;				
			vertices[index].pos = D3DXVECTOR3(x,0.0f,z);
		}
	}
	for (int i = 0; i < iterations; i++){
		float v = rand();
		float a = sin(v);
		float b = cos(v);
		// rand() / RAND_MAX gives a random number between 0 and 1.
		// therefore c will be a random number between -d/2 and d/2
		float c = RandF() * d - d/2;
		for(int i=0; i<gridWidth; i++){
			for(int j=0; j<gridDepth; j++){
				index = (gridWidth * i) + j;				
				if (a*i + b*j - c > 0){
					vertices[index].pos.y += displacementFactor;
				}
				else{
					vertices[index].pos.y -= displacementFactor;
				}
				heightData[index] = vertices[index].pos.y;//place in height data array
				if (heightData[index] > maxHeight){
					maxHeight = heightData[index];
				}
			}
		}
	}

	SmoothHeights(0.75f);SmoothHeights(0.75f);

	ComputeNormals();
	ComputeTextureCoords();

	ComputeIndices();

	//initialize the buffers with the index and vertex data
	if (!InitializeBuffers(indices, vertices))
		return false;

	return true;
}

bool Grid::GenerateGridDiamondSquare(int size, float displacementFactor, float roughnessConstant, bool wrap){
	ResetData();
	int index;
	float height;

	size = (int)(pow(2.0f,size))+1;
	
	if (size % 2 == 0)
		size += 1;

	gridWidth = size;
	gridDepth = size;

	mVertexCount = (gridWidth*gridDepth);
	vertices = new VertexNT[mVertexCount];

	heightData = new float[gridWidth*gridDepth];

	float dx = CELLSPACING;
	float halfWidth = (gridWidth-1)*dx*0.5f;
	float halfDepth = (gridDepth-1)*dx*0.5f;

 	for(int i=0; i<gridWidth; i++){
		float z = halfDepth - i*dx;

		for(int j=0; j<gridDepth; j++){		
			float x = -halfWidth + j*dx;
			index = (gridWidth * i) + j;				
			vertices[index].pos = D3DXVECTOR3(x,0.0f,z);
			heightData[index] = 0.0f;
		}
	}

	int i,j,step;
	float dispM = displacementFactor;
	float r = roughnessConstant; //roughness

	//set the four corners to the default value
	for (int i=0;i<gridWidth;i+=size-1){
		for (int j=0;j<gridDepth;j+=size-1){
			index = i*gridWidth + j;
			vertices[index].pos.y = dispM;
			heightData[index] = dispM;
			if (heightData[index] > maxHeight){
				maxHeight = heightData[index];
			}
		}
	}

	for (step = gridWidth-1; step > 1; step /= 2 ) {

		//diamond step
		for (i = 0;i<gridDepth-2;i+=step){
			for(j=0;j<gridWidth-2;j+=step) {	

				index = (i+step/2)*gridWidth + j + step/2;
				heightData[index] = 
					(TerrainHeight(i,j) + 
					TerrainHeight(i+step,j) + 
					TerrainHeight(i+step,j+step) + 
					TerrainHeight(i,j+step)) / 4.0f;
				heightData[index] += RandF(-dispM,dispM);
				vertices[index].pos.y = heightData[index];
				height =  heightData[index];
				if (heightData[index] > maxHeight){
					maxHeight = heightData[index];
				}
			}
		}
		
		//square step
		for (i = 0;i<gridDepth-2;i+=step){
			for(j=0;j<gridWidth-2;j+=step){
				int k,m;
				int x,z;

				x = i + step/2;
				z = j + step/2;

				k = x + step/2;
				m = z;
				if (k == gridDepth-1){
					if (wrap){
						heightData[k*gridWidth + m] = 
									(TerrainHeight(k,m+step/2) + 
									 TerrainHeight(k,m-step/2) + 
									 TerrainHeight(k-step/2,m) +
									 TerrainHeight(0,m)) / 4.0f;
					}
					else{
						heightData[k*gridWidth + m] = 
									(TerrainHeight(k,m+step/2) + 
									 TerrainHeight(k,m-step/2) + 
									 TerrainHeight(k-step/2,m)) / 3.0f;
					}
				}
				else{
					heightData[k*gridWidth + m] = 
									(TerrainHeight(k,m+step/2) + 
									TerrainHeight(k,m-step/2) + 
									TerrainHeight(k-step/2,m) + 
									TerrainHeight(k+step/2,m)) / 4.0f;
				}
				heightData[k*gridWidth + m] += RandF(-dispM,dispM);
				vertices[k*gridWidth + m].pos.y = heightData[k*gridWidth + m];
				if (heightData[index] > maxHeight){
					maxHeight = heightData[index];
				}

				k = x;
				m = z + step/2;

				if (m == gridWidth-1)
					if (wrap){
						heightData[k*gridWidth + m] = 
										(TerrainHeight(k,m-step/2) + 
										 TerrainHeight(k-step/2,m) + 
										 TerrainHeight(k+step/2,m) +
										 TerrainHeight(k,0)) / 4.0f;
					}
					else{
						heightData[k*gridWidth + m] = 
										(TerrainHeight(k,m-step/2) + 
										 TerrainHeight(k-step/2,m) + 
										 TerrainHeight(k+step/2,m)) / 3.0f;
					}
				else{
					heightData[k*gridWidth + m] = 
									(TerrainHeight(k,m+step/2) + 
									TerrainHeight(k,m-step/2) + 
									TerrainHeight(k-step/2,m) + 
									TerrainHeight(k+step/2,m)) / 4.0f;
				}
				heightData[k*gridWidth + m] += RandF(-dispM,dispM);
				vertices[k*gridWidth + m].pos.y = heightData[k*gridWidth + m];
				if (heightData[index] > maxHeight){
					maxHeight = heightData[index];
				}
	
				k = x - step/2;
				m = z;
				if (k == 0){
					if (wrap){
						heightData[k*gridWidth + m] = 
										(TerrainHeight(k,m+step/2) + 
										 TerrainHeight(k,m-step/2) + 
										 TerrainHeight(k+step/2,m) +
										 TerrainHeight(gridWidth-1,m)) / 4.0f;
					}
					else{
						heightData[k*gridWidth + m] = 
										(TerrainHeight(k,m+step/2) + 
										TerrainHeight(k,m-step/2) + 
										TerrainHeight(k+step/2,m)) / 3.0f;
					}
					heightData[k*gridWidth + m] += RandF(-dispM,dispM);
					vertices[k*gridWidth + m].pos.y = heightData[k*gridWidth + m];
					if (heightData[index] > maxHeight){
						maxHeight = heightData[index];
					}
				}

				m= z - step/2;
				k = x;
				if (m == 0){
					if (wrap){
						heightData[k*gridWidth + m] = 
										(TerrainHeight(k,m+step/2) + 
										TerrainHeight(k-step/2,m) + 
										TerrainHeight(k+step/2,m) +
										TerrainHeight(k+step/2,gridDepth-1)) / 4.0f;
					}
					else{
						heightData[k*gridWidth + m] = 
										(TerrainHeight(k,m+step/2) + 
										TerrainHeight(k-step/2,m) + 
										TerrainHeight(k+step/2,m)) / 3.0f;
					}
					heightData[k*gridWidth + m] += RandF(-dispM,dispM);
					vertices[k*gridWidth + m].pos.y = heightData[k*gridWidth + m];
					if (heightData[index] > maxHeight){
						maxHeight = heightData[index];
					}
				}				
			}
		}
		dispM *= r;		
	}
	SmoothHeights(0.75f);SmoothHeights(0.75f);

	ComputeNormals();
	ComputeTextureCoords();

	ComputeIndices();

	//initialize the buffers with the index and vertex data
	if (!InitializeBuffers(indices, vertices))
		return false;

	return true;
}

float Grid::TerrainHeight(int x,int z) {

	if (x > gridWidth-1)
		x -= (gridWidth-1);
	else if (x < 0)
		x += gridWidth-1;
	if (z > gridDepth-1)
		z -= (gridDepth-1);
	else if (z < 0)	
		z += gridDepth-1;
	assert(x>=0 && x < gridWidth);
	assert(z>=0 && z < gridDepth);
	return(heightData[x * gridWidth + z]);
}

//Smooth out each vertex depending on the nearest vertices to it using the float as the smoothing factor on how much to decrease/increase height
void Grid::SmoothHeights(float factor){	
	
	float k = factor;
	for (int i = 1; i < gridWidth; i++){
		for (int j = 0; j < gridDepth; j++){
			int position = j*gridWidth + i;
			heightData[position] = heightData[position - 1]*(1-k) + heightData[position] * k;
			vertices[position].pos.y = heightData[position];
		}
	}

	for (int i = gridWidth-2; i < -1; i--){
		for (int j = 0; j < gridDepth; j++){
			int position = j*gridWidth + i;
			heightData[position] = heightData[position+ 1]*(1-k) + heightData[position] * k;
			vertices[position].pos.y = heightData[position];
		}
	}

	for (int i = 0; i < gridWidth; i++){
		for (int j = 1; j < gridDepth; j++){
			int position = j*gridWidth + i;
			heightData[position] = heightData[(j-1)*gridWidth + i]*(1-k) + heightData[position] * k;
			vertices[position].pos.y = heightData[position];
		}
	}

	for (int i = 0; i < gridWidth; i++){
		for (int j = gridDepth; j < -1; j--){
			int position = j*gridWidth + i;
			heightData[position] = heightData[(j+1)*gridWidth + i]*(1-k) + heightData[position] * k;
			vertices[position].pos.y = heightData[position];
		}
	}
}

void Grid::AnimateTerrain(float dt){
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

void Grid::NormalizeHeightMap(){
	int i, j;
	maxHeight = 0;
	for(j=0; j<gridDepth; j++){
		for(i=0; i<gridWidth; i++){
			int index = (gridDepth * j) + i;
			float height = vertices[index].pos.y;
			height /= 15.0f;
			vertices[index].pos.y = height;
			if (height > maxHeight){
				maxHeight = height;
			}
			heightData[index] = height;//place in height data array
		}
	}
	return;
}

void Grid::ComputeIndices(){
	// Iterate over each quad and compute indices.
	mIndexCount = ((gridWidth-1)*(gridDepth-1)*6);
	indices = new DWORD[mIndexCount];
	int k = 0;
	bool switchPtrn = false;
	for(DWORD i = 0; i < gridWidth-1; ++i){
		for(DWORD j = 0; j < gridDepth-1; ++j){

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
void Grid::ComputeNormals()const{

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

void Grid::ComputeTextureCoords(const int repeatAmount)const{

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
bool Grid::InitializeBuffers(DWORD* indices,  VertexNT* vertices){

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

void Grid::AnimateUV(float dt){
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
	mTexMatrix = S*T;
}

float Grid::GetMaxHeight(){
	return maxHeight;
}

float Grid::GetHeight(float x, float z){
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