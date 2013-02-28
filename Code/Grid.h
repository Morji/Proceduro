#ifndef _GRID_H
#define _GRID_H

#include "GameObject.h"
#include "d3dUtil.h"
#include "TerrainLoader.h"
#include "SimplexNoise.h"

#define CELLSPACING		1.0f
#define	HEIGHT_FACTOR	0.2f;

class Grid : public GameObject
{
public:
	Grid(void);
	~Grid(void);

	bool GenerateGrid(int width, int depth);
	bool GenerateGridFromTGA(char* filename);
	bool GenerateHeightMap(bool keydown, int width, int depth);

	bool GenerateGridFaultLine(int iterations, float displacementFactor, int width, int depth);
	bool GenerateGridDiamondSquare(int size, float displacementFactor, float roughnessConstant, bool wrap);

	bool NoiseOverTerrain(float maxHeightToAdd, float smoothnessFactor);//perform a noise iteration over the terrain - this function requires that a terrain has already been generated

	float GetMaxHeight();

	float GetHeight(float x, float z);

	void  AnimateUV(float dt);

	void  AnimateTerrain(float dt);

private:
	bool InitializeBuffers(DWORD* indices,  VertexNT* vertices);

	void  ComputeIndices();
	void  ComputeNormals()const;				// computes the normals of the terrain on a per-vertex level
	void  ComputeTextureCoords(const int repeatAmount = 1)const;		// computes the texture coordinates of the terrain

	void  NormalizeHeightMap();
	void  SmoothHeights(float factor);	// smooths out the heights in the terrain by the given factor
	void  ResetData();

	float TerrainHeight(int x,int z);

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