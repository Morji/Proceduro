#include "Terrain.h"

Terrain::Terrain(void)
{
	maxHeight = 0.0f;
	heightData = nullptr;
	vertices = nullptr;
	indices = nullptr;
	mTerrainShader = 0;
	specularMap = 0;
	mTotalNodes = 0;
	for (int i = 0; i < 3; i++)
		diffuseMapRV[i] = 0;
	mTriangleCount = 0;
	mNodeCount = 0;
	mTrianglesPerNode = 0;
	mTrianglesLooped = 0;
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
	if (mBaseNode){
		delete mBaseNode;
		mBaseNode = nullptr;
	}
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

void Terrain::Render(Frustum* frustum, D3DXMATRIX worldMatrix,D3DXMATRIX viewMatrix,D3DXMATRIX projectionMatrix, Vector3f eyePos, Light light, int lightType){
	D3DXMATRIX mObjMatrix;
	mTransform->GetTransformMatrix(mObjMatrix);
	mObjMatrix*=worldMatrix;

	// Reset the number of triangles that are drawn for this frame.
	mNodeCount = 0;
	
	mTerrainShader->SetShaderParameters(md3dDevice,mObjMatrix,viewMatrix,projectionMatrix,
							eyePos,
							light,
							diffuseMapRV[0]->GetTexture(),
							diffuseMapRV[1]->GetTexture(),
							diffuseMapRV[2]->GetTexture(),
							maxHeight,
							lightType);

	RenderNode(mBaseNode,frustum);
}

void Terrain::RenderNode(TerrainNode* node, Frustum* frustum){
	bool result;
	int count;

	// Check to see if the node can be viewed, height doesn't matter in a quad tree.
	result = frustum->CheckCube(node->positionX, 0.0f, node->positionZ, (node->diameter / 2.0f));
	// If it can't be seen then none of its children can either so don't continue down the tree, this is where the speed is gained.
	if(!result){
		return;
	}

	// If it can be seen then check all four child nodes to see if they can also be seen.
	count = 0;
	for(int i = 0; i < 4; i++){
		if(node->childNodes[i] != 0){
			count++;
			RenderNode(node->childNodes[i], frustum);
		}
	}

	// If there were any children nodes then there is no need to continue as parent nodes won't contain any triangles to render.
	if(count != 0){
		return;
	}

	// Otherwise if this node can be seen and has triangles in it then render these triangles.

	// Set vertex buffer stride and offset.
	node->RenderBuffers(md3dDevice);

	// Determine the number of indices in this node.
	int indexCount = node->GetIndexCount();

	// Call the terrain shader to render the polygons in this node.
	mTerrainShader->Render(md3dDevice, indexCount);

	mNodeCount++;
}

bool Terrain::CreateTerrain(TerrainGenerator *generator){
	if (!generator){
		return false;
	}

	ResetData();
	gridWidth = generator->GetWidth();
	gridDepth = generator->GetDepth();

	int mVertexCount = (gridWidth*gridDepth);
	vertices = new VertexNT[mVertexCount];

	heightData = new float[mVertexCount];

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

	ComputeMeshQuadTree();	

	delete [] vertices;
	vertices = nullptr;
	delete [] indices;
	indices = nullptr;

	return true;
}

void Terrain::ComputeMeshQuadTree(){
	int indexCount, vertexCount;
	float centerX, centerZ, meshDiameter;
	float maxWidth, maxDepth, minWidth, minDepth, width, depth, maxX, maxZ;

	vertexCount = gridWidth*gridDepth;
	indexCount = ((gridWidth-1)*(gridDepth-1)*6);
	mTriangleCount = indexCount/3;

	//find out how many nodes this terrain is going to be split into
	mTrianglesPerNode = mTriangleCount;
	while (mTrianglesPerNode > MAX_TRIANGLES){
		mTrianglesPerNode /= 2;
	}
	mTotalNodes = mTriangleCount/mTrianglesPerNode;

	// Initialize the center position of the mesh to zero.
	centerX = 0.0f;
	centerZ = 0.0f;

	// Sum all the vertices in the mesh.
	for(int i=0; i<vertexCount; i++){
		centerX += vertices[i].pos.x;
		centerZ += vertices[i].pos.z;
	}

	// And then divide it by the number of vertices to find the mid-point of the mesh.
	centerX = centerX / (float)vertexCount;
	centerZ = centerZ / (float)vertexCount;

	// Initialize the maximum and minimum size of the mesh.
	maxWidth = 0.0f;
	maxDepth = 0.0f;

	minWidth = fabsf(vertices[0].pos.x - centerX);
	minDepth = fabsf(vertices[0].pos.z - centerZ);

	// Go through all the vertices and find the maximum and minimum width and depth of the mesh.
	for(int i=0; i<vertexCount; i++){
		width = fabsf(vertices[i].pos.x - centerX);	
		depth = fabsf(vertices[i].pos.z - centerZ);	

		if(width > maxWidth) { maxWidth = width; }
		if(depth > maxDepth) { maxDepth = depth; }
		if(width < minWidth) { minWidth = width; }
		if(depth < minDepth) { minDepth = depth; }
	}

	// Find the absolute maximum value between the min and max depth and width.
	maxX = (float)max(fabs(minWidth), fabs(maxWidth));
	maxZ = (float)max(fabs(minDepth), fabs(maxDepth));
	
	// Calculate the maximum diameter of the mesh.
	meshDiameter = max(maxX, maxZ) * 2.0f;

	// Create the parent node for the quad tree.
	mBaseNode = new TerrainNode();
	if(!mBaseNode){
		return;
	}

	// Recursively build the quad tree based on the vertex list data and mesh dimensions.
	CreateTreeNode(mBaseNode, centerX, centerZ, meshDiameter);
}

void Terrain::CreateTreeNode(TerrainNode *node, float positionX, float positionZ, float diameter){
	int numTriangles, count, vertexCount, index, vertexIndex;
	float offsetX, offsetZ;
	VertexNT* nodeVertices;
	DWORD* nodeIndices;
	bool result;	

	// Store the node position and size.
	node->positionX = positionX;
	node->positionZ = positionZ;
	node->diameter = diameter;

	// Count the number of triangles that are inside this node.
	numTriangles = GetTriangleCount(positionX, positionZ, diameter);

	// Case 1: If there are no triangles in this node then return as it is empty and requires no processing.
	if(numTriangles == 0){
		return;
	}

	// Case 2: If there are too many triangles in this node then split it into four equal sized smaller tree nodes.
	if(numTriangles > MAX_TRIANGLES){
		for(int i = 0; i < 4; i++){
			// Calculate the position offsets for the new child node.
			offsetX = (((i % 2) < 1) ? -1.0f : 1.0f) * (diameter / 4.0f);
			offsetZ = (((i % 4) < 2) ? -1.0f : 1.0f) * (diameter / 4.0f);

			// See if there are any triangles in the new node.
			count = GetTriangleCount((positionX + offsetX), (positionZ + offsetZ), (diameter / 2.0f));
			if(count > 0){
				// If there are triangles inside where this new node would be then create the child node.
				node->childNodes[i] = new TerrainNode();

				// Extend the tree starting from this new child node now.
				CreateTreeNode(node->childNodes[i], (positionX + offsetX), (positionZ + offsetZ), (diameter / 2.0f));
			}
		}
		return;
	}

	mNodeCount++;
	std::cout << "Initializing node " << mNodeCount << " out of " << mTotalNodes << std::endl;

	// Case 3: If this node is not empty and the triangle count for it is less than the max then 
	// this node is at the bottom of the tree so create the list of triangles to store in it.
	node->triangleCount = numTriangles;

	// Calculate the number of vertices.
	//vertexCount = (diameter+1)*(diameter+1);
	vertexCount = numTriangles*3;
	// Create the vertex array.
	nodeVertices = new VertexNT[vertexCount];
	int indexCount = numTriangles*3;
	// Initialize the index for this new vertex and index array.
	index = 0;
	nodeIndices = new DWORD[indexCount];
	// Go through all the triangles in the vertex list.
	for(int loop = 0; loop < mTriangleCount; loop++){
		// If the triangle is inside this node then add it to the vertex array.
		result = IsTriangleContained(loop, positionX, positionZ, diameter);
		if(result == true){			
			// Calculate the index into the terrain vertex list.
			vertexIndex = loop * 3;

			// Get the three vertices of this triangle from the vertex list.
			nodeVertices[index].pos = vertices[indices[vertexIndex]].pos;
			nodeVertices[index].texC = vertices[indices[vertexIndex]].texC;
			nodeVertices[index].normal = vertices[indices[vertexIndex]].normal;
			nodeIndices[index] = index;
			index++;

			vertexIndex++;
			nodeVertices[index].pos = vertices[indices[vertexIndex]].pos;
			nodeVertices[index].texC = vertices[indices[vertexIndex]].texC;
			nodeVertices[index].normal = vertices[indices[vertexIndex]].normal;
			nodeIndices[index] = index;
			index++;

			vertexIndex++;
			nodeVertices[index].pos = vertices[indices[vertexIndex]].pos;
			nodeVertices[index].texC = vertices[indices[vertexIndex]].texC;
			nodeVertices[index].normal = vertices[indices[vertexIndex]].normal;
			nodeIndices[index] = index;
			index++;
		}
	}	
	mTrianglesLooped += numTriangles;
	/*int k = 0;
	//bool switchPtrn = false;
	for(int i = 0; i < diameter; ++i){
		for(int j = 0; j < diameter; ++j){

			//if (switchPtrn == false){
				nodeIndices[k] = i*(diameter+1)+j;
				nodeIndices[k+1] = i*(diameter+1)+j+1;
				nodeIndices[k+2] = (i+1)*(diameter+1)+j;

				nodeIndices[k+3] = (i+1)*(diameter+1)+j;
				nodeIndices[k+4] = i*(diameter+1)+j+1;
				nodeIndices[k+5] = (i+1)*(diameter+1)+j+1;
			/*}
			else{
				nodeIndices[k] = i*(diameter+1)+j;
				nodeIndices[k+1] = i*(diameter+1)+j+1;
				nodeIndices[k+2] = (i+1)*(diameter+1)+j+1;

				nodeIndices[k+3] = (i+1)*(diameter+1)+j+1;
				nodeIndices[k+4] = (i+1)*(diameter+1)+j;
				nodeIndices[k+5] = i*(diameter+1)+j;
			}
			
			switchPtrn = !switchPtrn;
			k += 6; // next quad
		}
		//switchPtrn = ((int)(diameter+1) % 2 == 1) ?  !switchPtrn : switchPtrn;
	}*/

	result = node->InitializeBuffers<VertexNT>(md3dDevice,nodeIndices,nodeVertices,vertexCount,indexCount);

	// Release the vertex and index arrays now that the data is stored in the buffers in the node.
	delete [] nodeVertices;
	nodeVertices = 0;

	delete [] nodeIndices;
	nodeIndices = 0;
}

int	Terrain::GetTriangleCount(float positionX, float positionZ, float diameter){
	int count = 0;
	bool result;

	// Go through all the triangles in the entire mesh and check which ones should be inside this node.
	for(int i=0; i<mTriangleCount; i++)
	{
		// If the triangle is inside the node then increment the count by one.
		result = IsTriangleContained(i, positionX, positionZ, diameter);
		if(result == true){
			count++;
		}
	}

	return count;
}

bool Terrain::IsTriangleContained(int index, float positionX, float positionZ, float diameter){
	
	float radius;
	int vertexIndex;
	float x1, z1, x2, z2, x3, z3;
	float minimumX, maximumX, minimumZ, maximumZ;

	// Calculate the radius of this node.
	radius = diameter / 2.0f;

	// Get the index into the vertex list.
	vertexIndex = index * 3;

	x1 = vertices[indices[vertexIndex]].pos.x;
	z1 = vertices[indices[vertexIndex]].pos.z;
	vertexIndex++;

	x2 = vertices[indices[vertexIndex]].pos.x;
	z2 = vertices[indices[vertexIndex]].pos.z;
	vertexIndex++;

	x3 = vertices[indices[vertexIndex]].pos.x;
	z3 = vertices[indices[vertexIndex]].pos.z;

	// Check to see if the minimum of the x coordinates of the triangle is inside the node.
	minimumX = min(x1, min(x2, x3));
	if(minimumX > (positionX + radius)){
		return false;
	}

	// Check to see if the maximum of the x coordinates of the triangle is inside the node.
	maximumX = max(x1, max(x2, x3));
	if(maximumX < (positionX - radius)){
		return false;
	}

	// Check to see if the minimum of the z coordinates of the triangle is inside the node.
	minimumZ = min(z1, min(z2, z3));
	if(minimumZ > (positionZ + radius)){
		return false;
	}

	// Check to see if the maximum of the z coordinates of the triangle is inside the node.
	maximumZ = max(z1, max(z2, z3));
	if(maximumZ < (positionZ - radius)){
		return false;
	}

	return true;
}

void Terrain::ResetData(){
	maxHeight = 0.0f;
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

void Terrain::ComputeIndices(){
	// Iterate over each quad and compute indices.
	int mIndexCount = ((gridWidth-1)*(gridDepth-1)*6);
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

int Terrain::GetDrawCount(){
	return mNodeCount;
}