#include "GeometryBuilder.h"

bool CreateSphereVertsAndIndices(int nRings, int nSegments, Vector3f *verts, DWORD *indices){
	int numVertices = (nRings + 1) * (nSegments + 1);
	int numIndices = 2 * nRings * (nSegments + 1); 

	if (!verts){
		return false;
	}
	if (!indices){
		return false;
	}
	
	//Establish constants used in sphere generation   
    float rDeltaRingAngle = (D3DX_PI / nRings);   
    float rDeltaSegAngle = (2.0f * D3DX_PI / nSegments);   

	int currVert = 0;
	int currIndex = 0;
	int wVertexIndex = 0;
	//Generate the group of rings for the sphere   
    for(int nCurrentRing = 0; nCurrentRing < nRings + 1; nCurrentRing++){ 

        float r0 = sinf(nCurrentRing * rDeltaRingAngle);   
        float y0 = cosf(nCurrentRing * rDeltaRingAngle);   
   
        //Generate the group of segments for the current ring   
        for(int nCurrentSegment = 0; nCurrentSegment < nSegments + 1; nCurrentSegment++){   
            FLOAT x0 = r0 * sinf(nCurrentSegment * rDeltaSegAngle);   
            FLOAT z0 = r0 * cosf(nCurrentSegment * rDeltaSegAngle);   
   
			verts[currVert] = Vector3f(x0,y0,z0);   
            currVert++;   
               
            //Add two indices except for the last ring    
            if(nCurrentRing != nRings){   
                indices[currIndex] = wVertexIndex;    
                currIndex++;   
                   
                indices[currIndex] = wVertexIndex + (DWORD)(nSegments + 1);    
                currIndex++;   
                   
                wVertexIndex++;    
            }   
        }   
    }   
}

bool BuildSphere(int nRings, int nSegments, ID3D10Device* device, Renderer *targetRenderer){
	int numVertices = (nRings + 1) * (nSegments + 1);
	int numIndices = 2 * nRings * (nSegments + 1); 
	Vector3f *vertices = new Vector3f[numVertices];
	DWORD *indices = new DWORD[numIndices];
	CreateSphereVertsAndIndices(nRings,nSegments,vertices,indices);

	bool result = targetRenderer->InitializeBuffers(device,indices,vertices,sizeof(Vector3f),numVertices,numIndices);

	//cleanup
	delete [] vertices;
	vertices = nullptr;
	delete [] indices;
	indices = nullptr;

	if (!result){
		return false;
	}
	return true;
}

bool BuildColoredSphere(int nRings, int nSegments, ID3D10Device* device, Renderer *targetRenderer, Color color){
	int numVertices = (nRings + 1) * (nSegments + 1);
	int numIndices = 2 * nRings * (nSegments + 1); 
	Vector3f *vertices = new Vector3f[numVertices];
	DWORD *indices = new DWORD[numIndices];
	CreateSphereVertsAndIndices(nRings,nSegments,vertices,indices);

	VertexC *coloredVerts = new VertexC[numVertices];
	for (int i = 0; i < numVertices; i++){
		coloredVerts[i].pos = vertices[i];
		coloredVerts[i].color = color;
	}

	bool result = targetRenderer->InitializeBuffers(device,indices,coloredVerts,sizeof(VertexC),numVertices,numIndices);

	//cleanup
	delete [] vertices;
	vertices = nullptr;
	delete [] indices;
	indices = nullptr;
	delete [] coloredVerts;
	coloredVerts = nullptr;

	if (!result){
		return false;
	}
	return true;
}

bool BuildColoredCube(ID3D10Device* device, Renderer *targetRenderer, Color color){
	// Load the vertex array with data.
	VertexC vertices[] = {
		//POS						   //COLOR					   
		//back
		{Vector3f(-1.0f, -1.0f, -1.0f),	color}, // Bottom left.	
		{Vector3f(-1.0f, +1.0f, -1.0f), color}, // Top Left
		{Vector3f(+1.0f, +1.0f, -1.0f), color}, // Top Right
		{Vector3f(+1.0f, -1.0f, -1.0f), color}, // Bottom Right

		//front
		{Vector3f(-1.0f, -1.0f, 1.0f), color}, // Bottom left.	
		{Vector3f(-1.0f, +1.0f, 1.0f), color}, // Top Left
		{Vector3f(+1.0f, +1.0f, 1.0f), color}, // Top Right
		{Vector3f(+1.0f, -1.0f, 1.0f), color}, // Bottom Right
	};

	DWORD indices[] ={
		// Back face
        0,1,2,
        0,2,3,
        // Front face
        4,6,5,
        4,7,6,
        // Left face
        4,5,1,
        4,1,0,
        // Right face
        3,2,6,
        3,6,7,
        // Top face
        1,5,6,
        1,6,2,
        // Bottom face
        4,0,3,
        4,3,7
	};

	// Set the number of vertices in the vertex array.
	int vertexCount = sizeof(vertices)/sizeof(vertices[0]);
	// Set the number of indices in the index array.
	int indexCount = sizeof(indices)/sizeof(indices[0]);

	bool result = targetRenderer->InitializeBuffers(device,indices,vertices,sizeof(VertexC),vertexCount,indexCount);

	if (!result){
		return false;
	}
	return true;
}