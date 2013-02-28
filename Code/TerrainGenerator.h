/*************************************************************
TerrainGenerator Class: Used to supply the Terrain class with
the necessary information to create a terrain object

Author: Valentin Hinov
Date: 24/02/2013
Version: 1.0

Exposes: TerrainGenerator
Supported generation methods: 
* Load Heightmap from a .TGA file
* Generate a terrain via fault line algorithm
* Generate a terrain via mid point displacement(diamond-square) algorithm
* Perform a noise operation over the terrain using simplex noise
**************************************************************/

#ifndef _TERRAINGENERATOR_H
#define _TERRAINGENERATOR_H

#include "d3dUtil.h"
#include "TgaHeader.h"
#include "SimplexNoise.h"

const unsigned char UTGAcompare[12] = {0,0,2, 0,0,0,0,0,0,0,0,0};	// Uncompressed TGA Header

class TerrainGenerator{
public:
	TerrainGenerator();
	~TerrainGenerator();

	bool	GenerateFromTGA(char* filename);

	bool	GenerateFaultLine(int iterations, float displacementFactor, int width, int depth);
	bool	GenerateGridDiamondSquare(int size, float displacementFactor, float roughnessConstant, bool wrap);

	//perform a noise iteration over the terrain - this function requires that a terrain has already been generated
	bool	NoiseOverTerrain(float maxHeightToAdd, float smoothnessFactor);

	int		GetWidth();
	int		GetDepth();
	float	GetHeight(int x, int z);		// return the height value associated with the X Z coordinate
	float	GetMaxHeight();

private:
	FILE	*LoadTGA(char* filename);		// loads in terrain heightmap to a file returns a pointer to the file
	bool	ComputeHeights(FILE * fTGA);	// computes the heights from the TGA file
	void	SmoothHeights(float factor);	// smooths out the heights in the terrain by the given factor
	void	Reset();

private:
	float	*mHeights;	// an array to hold the height values from the heightmap
	float	mMaxHeight;

	int		mTerrainWidth;
	int		mTerrainDepth;

	TGAHeader Tgaheader;
};

#endif