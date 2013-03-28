/*************************************************************
Terrain Class: Exposes a terrain object. In order to create 
the terrain this class must be supplied with a TerrainGenerator
object that will provide the necessary information

Author: Valentin Hinov
Date: 24/02/2013
Version: 2.0

Exposes: Terrain
Supported methods:
* GetMaxHeight - the maximum height of terrain
* GetHeight(float x, float y) - get the height of the terrain at given coordinates
* GetRandomPoint - get a random point on the surface of the terrain
**************************************************************/

#ifndef _TERRAIN_H
#define _TERRAIN_H

#include <iostream>
#include "BaseGameObject.h"
#include "TerrainHeightShader.h"
#include "d3dUtil.h"
#include "TextureLoader.h"
#include "TerrainNode.h"
#include "Frustum.h"
#include "TerrainGenerator.h"

#define CELLSPACING		1.0f
#define	HEIGHT_FACTOR	0.2f

class Terrain : public BaseGameObject
{
public:
	Terrain(void);
	~Terrain(void);

	bool		Initialize(ID3D10Device* device, HWND hwnd);
	bool		CreateTerrain(TerrainGenerator *generator);

	float		GetMaxHeight();
	float		GetHeight(float x, float z);

	// get a random point on the surface of the terrain
	Vector3f	GetRandomPoint();
	int			GetDrawCount();

	void		Render(Frustum* frustum, D3DXMATRIX worldMatrix,D3DXMATRIX viewMatrix,D3DXMATRIX projectionMatrix, Vector3f eyePos, Light light, int lightType);

private:
	void  RenderNode(TerrainNode* node, Frustum* frustum);
	void  ComputeIndices();
	void  ComputeMeshQuadTree();
	void  CreateTreeNode(TerrainNode *node, float positionX, float positionZ, float diameter);
	int	  GetTriangleCount(float positionX, float positionZ, float diameter);
	bool  IsTriangleContained(int index, float positionX, float positionZ, float diameter);

	void  ComputeNormals()const;				// computes the normals of the terrain on a per-vertex level
	void  ComputeTextureCoords(const int repeatAmount = 1)const;		// computes the texture coordinates of the terrain

	void  ResetData();

	TextureLoader	*specularMap;
	TextureLoader	*diffuseMapRV[3];

	TerrainHeightShader	*mTerrainShader;
	ID3D10Device		*md3dDevice;

	TerrainNode			*mBaseNode;
private:	
	int				mTotalNodes;
	int				mTriangleCount;
	int				mNodeCount;
	float			animCoeff;
	DWORD			*indices;
	VertexNT		*vertices;
	int				gridWidth;
	int				gridDepth;
	float			maxHeight;

	Vector2f		texOffset;
	float			*heightData;			//array containing the height data for ease of access for terrain collision

	bool			terrainGeneratedToggle;
	
private:
	static const int SUBDIVISIONS = 4;
	int MAX_TRIANGLES;
};

#endif