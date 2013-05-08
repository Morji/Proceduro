#include "Terrain.h"

//enable/disable debugging
//#define DEBUG

Terrain::Terrain(void)
{
	maxHeight = 0.0f;
	heightData = nullptr;
	vertices = nullptr;
	indices = nullptr;
	mTerrainShader = nullptr;
	specularMap = 0;
	mTotalNodes = 0;
	for (int i = 0; i < 3; i++){
		diffuseMapRV[i] = nullptr;
	}
	mNodeCount = 0;

	mTransform = new Transform(this);
	AddComponent(mTransform,TRANSFORM);
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

void Terrain::Render(ID3DObject *d3dObject,Frustum* frustum, D3DXMATRIX viewMatrix, Vector3f eyePos, Light light, int lightType){
	D3DXMATRIX mObjMatrix;
	d3dObject->GetWorldMatrix(mObjMatrix);
	mTransform->GetTransformMatrix(mObjMatrix);

	// Reset the number of triangles that are drawn for this frame.
	mNodeCount = 0;
	
	mTerrainShader->SetShaderParameters(d3dObject->GetDevice(),mObjMatrix,viewMatrix,*d3dObject->GetProjectionMatrix(),
							eyePos,
							light,
							diffuseMapRV[0]->GetTexture(),
							diffuseMapRV[1]->GetTexture(),
							diffuseMapRV[2]->GetTexture(),
							maxHeight,
							lightType);

	RenderNode(mBaseNode, d3dObject, frustum, &viewMatrix);
}

void Terrain::RenderNode(TerrainNode* node, ID3DObject *d3dObject, Frustum* frustum, D3DXMATRIX *viewMatrix){
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
			RenderNode(node->childNodes[i], d3dObject, frustum, viewMatrix);
			
		}
	}

	// If there were any children nodes then there is no need to continue as parent nodes won't contain any triangles to render.
	if(count != 0){
		return;
	}

	// Otherwise if this node can be seen and has triangles in it then render these triangles.

	// Set vertex buffer stride and offset.
	#ifdef DEBUG
	node->DrawBounds(d3dObject,*viewMatrix,maxHeight);
	#endif
	node->nodeRenderer->RenderBuffers(md3dDevice);

	// Determine the number of indices in this node.
	int indexCount = node->nodeRenderer->GetIndexCount();

	// Call the terrain shader to render the polygons in this node.
	mTerrainShader->Render(md3dDevice, indexCount);

	mNodeCount++;
}

bool Terrain::CreateTerrain(TerrainGenerator *generator){
	if (!generator){
		return false;
	}
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

	//find out how many nodes this terrain is going to be split into
	mVerteciesPerNode = vertexCount;
	while (mVerteciesPerNode > MAX_VERTICES){
		mVerteciesPerNode /= 2;
	}
	mTotalNodes = vertexCount/mVerteciesPerNode;

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
	int vertexCount, index;
	float offsetX, offsetZ;
	VertexNT* nodeVertices;
	DWORD* nodeIndices;
	bool result;	
	int intDiameter = (int)diameter;

	// Store the node position and size.
	node->positionX = positionX;
	node->positionZ = positionZ;
	node->diameter = diameter;

	// Count the number of triangles that are inside this node.
	vertexCount = GetVertexCount(positionX, positionZ, diameter);

	// Case 1: If there are no vertices in this node then return as it is empty and requires no processing.
	if(vertexCount == 0){
		return;
	}

	// Case 2: If there are too many vertices in this node then split it into four equal sized smaller tree nodes.
	if(vertexCount > MAX_VERTICES){
		for(int i = 0; i < 4; i++){
			// Calculate the position offsets for the new child node.
			offsetX = (((i % 2) < 1) ? -1.0f : 1.0f) * (diameter / 4.0f);
			offsetZ = (((i % 4) < 2) ? -1.0f : 1.0f) * (diameter / 4.0f);

			// See if there are any vertices in the new node.
			vertexCount = GetVertexCount((positionX + offsetX), (positionZ + offsetZ), (diameter / 2.0f));
			if(vertexCount > 0){
				// If there are vertices inside where this new node would be then create the child node.
				node->childNodes[i] = new TerrainNode();

				// Extend the tree starting from this new child node now.
				CreateTreeNode(node->childNodes[i], (positionX + offsetX), (positionZ + offsetZ), (diameter / 2.0f));
			}
		}
		return;
	}

	mNodeCount++;
	std::cout << "Initializing node " << mNodeCount << " out of " << mTotalNodes << std::endl;

	// Case 3: If this node is not empty and the vertex count for it is less than the max then 
	// this node is at the bottom of the tree so create the list of vertecies to store in it.
	// Create the vertex array.
	nodeVertices = new VertexNT[vertexCount];	
	// Initialize the index for this new vertex array.
	index = 0;
	int totalVertexCount = gridWidth*gridDepth;
	
	// Go through all the vertcies in the vertex list.
	for(int loop = 0; loop < totalVertexCount; loop++){
		// If the vertex is inside this node then add it to the vertex array.
		result = IsVertexContained(loop, positionX, positionZ, diameter);
		if(result == true){			
			// Calculate the index into the terrain vertex list.
			nodeVertices[index].pos = vertices[loop].pos;
			nodeVertices[index].texC = vertices[loop].texC;
			nodeVertices[index].normal = vertices[loop].normal;
			index++;
		}
	}

	int k = 0;
	bool switchPtrn = false;
	
	int indexCount = intDiameter*intDiameter*6;
	
	nodeIndices = new DWORD[indexCount];
	for(int i = 0; i < intDiameter; ++i){
		for(int j = 0; j < intDiameter; ++j){

			if (switchPtrn == false){
				nodeIndices[k] = i*(intDiameter+1)+j;
				nodeIndices[k+1] = i*(intDiameter+1)+j+1;
				nodeIndices[k+2] = (i+1)*(intDiameter+1)+j;

				nodeIndices[k+3] = (i+1)*(intDiameter+1)+j;
				nodeIndices[k+4] = i*(intDiameter+1)+j+1;
				nodeIndices[k+5] = (i+1)*(intDiameter+1)+j+1;
			}
			else{
				nodeIndices[k] = i*(intDiameter+1)+j;
				nodeIndices[k+1] = i*(intDiameter+1)+j+1;
				nodeIndices[k+2] = (i+1)*(intDiameter+1)+j+1;

				nodeIndices[k+3] = (i+1)*(intDiameter+1)+j+1;
				nodeIndices[k+4] = (i+1)*(intDiameter+1)+j;
				nodeIndices[k+5] = i*(intDiameter+1)+j;
			}
			
			switchPtrn = !switchPtrn;
			k += 6; // next quad
		}
		switchPtrn = ((intDiameter+1) % 2 == 1) ?  !switchPtrn : switchPtrn;
	}

	result = node->nodeRenderer->InitializeBuffers(md3dDevice,nodeIndices,nodeVertices,sizeof(VertexNT),vertexCount,indexCount);


	// Release the vertex and index arrays now that the data is stored in the buffers in the node.
	delete [] nodeIndices;
	nodeIndices = nullptr;

	delete [] nodeVertices;
	nodeVertices = nullptr;
}

int	Terrain::GetVertexCount(float positionX, float positionZ, float diameter)const{
	int count = 0;
	bool result;

	// Go through all the vertices in the entire mesh and check which ones should be inside this node.
	for(int i=0; i<gridWidth*gridDepth; i++){
		// If the vertex is inside the node then increment the count by one.
		result = IsVertexContained(i, positionX, positionZ, diameter);
		if(result == true){
			count++;
		}
	}

	return count;
}

bool Terrain::IsVertexContained(int index, int positionX, int positionZ, float diameter)const{
	
	float radius = diameter / 2.0f;

	Vector3f pos = vertices[index].pos;

	// Check to see if the x coordinates of the vertex is inside the node.
	if(pos.x > (positionX + radius) || pos.x < (positionX - radius)){
		return false;
	}

	// Check to see if the z coordinates of the vertex is inside the node.
	if(pos.z > (positionZ + radius) || pos.z  < (positionZ - radius)){
		return false;
	}

	return true;
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

float Terrain::GetMaxHeight()const{
	return maxHeight;
}

float Terrain::GetHeight(float x, float z)const{
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

Vector3f Terrain::GetRandomPoint()const{
	float dx = CELLSPACING;
	float halfWidth = (gridWidth-1)*dx*0.5f;
	float halfDepth = (gridDepth-1)*dx*0.5f;

	float z = RandF(-(float)gridWidth/2,(float)gridWidth/2);		
	float x = RandF(-(float)gridDepth/2,(float)gridDepth/2);

	float y = GetHeight(x,z);	

	x+=mTransform->position.x;
	z+=mTransform->position.z;

	return Vector3f(x,y,z);
}

int Terrain::GetDrawCount()const{
	return mNodeCount;
}