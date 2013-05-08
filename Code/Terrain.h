/**************************************************************
Terrain Class: Exposes a terrain object. In order to create 
the terrain this class must be supplied with a TerrainGenerator
object that will provide the necessary information. This terrain
provides it's own LOD and frustum culling operations by means
of a Quad Tree

Author: Valentin Hinov
Date: 24/02/2013
Version: 2.0

Exposes: Terrain
Supported methods:
* GetMaxHeight - the maximum height of terrain
* GetHeight(float x, float y) - get the height of the terrain at given coordinates
* GetRandomPoint - get a random point on the surface of the terrain
***************************************************************/

#ifndef _TERRAIN_H
#define _TERRAIN_H

#include <iostream>
#include <vector>
#include "BaseGameObject.h"
#include "TerrainHeightShader.h"
#include "ID3DObject.h"
#include "TextureLoader.h"
#include "TerrainNode.h"
#include "Frustum.h"
#include "Transform.h"
#include "TerrainGenerator.h"

#define CELLSPACING		1.0f
#define	HEIGHT_FACTOR	0.2f
#define MAX_VERTICES   10000 // max vertices per node

using namespace std;

class Terrain : public BaseGameObject
{
public:
	Terrain(void);
	~Terrain(void);

	bool		Initialize(ID3D10Device* device, HWND hwnd);
	bool		CreateTerrain(TerrainGenerator *generator);

	float		GetMaxHeight()const;
	float		GetHeight(float x, float z)const;
	
	Vector3f	GetRandomPoint()const; // get a random point on the surface of the terrain
	int			GetDrawCount()const; // returns the amount of terrain nodes being drawn

	void		Render(ID3DObject *d3dObject,Frustum* frustum, D3DXMATRIX viewMatrix, Vector3f eyePos, Light light, int lightType);

// Components
private:
	Transform	*mTransform;

private:
	//Initial terrain computing functions
	void  ComputeIndices();
	void  ComputeNormals()const;				// computes the normals of the terrain on a per-vertex level
	void  ComputeTextureCoords(const int repeatAmount = 1)const;		// computes the texture coordinates of the terrain

	void  RenderNode(TerrainNode* node, ID3DObject *d3dObject, Frustum* frustum, D3DXMATRIX *viewMatrix);
	
	void  ComputeMeshQuadTree();
	void  CreateTreeNode(TerrainNode *node, float positionX, float positionZ, float diameter);
	int	  GetVertexCount(float positionX, float positionZ, float diameter)const;
	bool  IsVertexContained(int index, int positionX, int positionZ, float diameter)const;

	TextureLoader		*specularMap;
	TextureLoader		*diffuseMapRV[3];

	TerrainHeightShader	*mTerrainShader;
	ID3D10Device		*md3dDevice;

	TerrainNode			*mBaseNode;

private:	
	int				mVerteciesPerNode;
	int				mTotalNodes;
	int				mNodeCount;
	DWORD			*indices;
	VertexNT		*vertices;
	int				gridWidth;
	int				gridDepth;
	float			maxHeight;

	float			*heightData;			//array containing the height data for ease of access for terrain collision	
};

#endif