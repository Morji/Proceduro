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

#include "BaseGameObject.h"
#include "TerrainHeightShader.h"
#include "d3dUtil.h"
#include "TextureLoader.h"
#include "TerrainGenerator.h"

#define CELLSPACING		1.0f
#define	HEIGHT_FACTOR	0.2f;

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

	void		AnimateUV(float dt);
	void		AnimateTerrain(float dt);

	void		Render(D3DXMATRIX worldMatrix,D3DXMATRIX viewMatrix,D3DXMATRIX projectionMatrix, Vector3f eyePos, Light light, int lightType);

private:

	bool InitializeBuffers(DWORD* indices,  VertexNT* vertices);
	void RenderBuffers();
	void  ComputeIndices();
	void  ComputeNormals()const;				// computes the normals of the terrain on a per-vertex level
	void  ComputeTextureCoords(const int repeatAmount = 1)const;		// computes the texture coordinates of the terrain

	void  ResetData();

	TextureLoader	*specularMap;
	TextureLoader	*diffuseMapRV[3];

	TerrainHeightShader	*mTerrainShader;
	ID3D10Device		*md3dDevice;

	DWORD				mVertexCount;
	DWORD				mIndexCount;
	ID3D10Buffer		*mVB;
	ID3D10Buffer		*mIB;

	unsigned int		stride;
	unsigned int		offset;
private:	
	float			animCoeff;
	DWORD			*indices;
	VertexNT		*vertices;
	int				gridWidth;
	int				gridDepth;
	float			maxHeight;

	Vector2f		texOffset;
	float			*heightData;			//array containing the height data for ease of access for terrain collision

	bool			terrainGeneratedToggle;
};

#endif