/*************************************************************
Tree Class: Used to instantiate and create a tree object via 
procedural methods.

Author: Valentin Hinov
Date: 21/02/2013
Version: 1.0

Exposes: Tree
Supported methods: Space Exploration
**************************************************************/

#ifndef _H_TREE_H
#define _H_TREE_H

#include <vector>

#include "CylinderObject.h"
#include "TexShader.h"
#include "TreeSegment.h"

class Tree{
public:
	Tree(void);
	Tree(ID3D10Device *d3dDevice, HWND hwnd, Vector3f position);

	bool GenerateTreeSpaceExploration(float minGrowthSize, float startRadius, float minInfluenceDist, float maxInfluenceDist, float maxHeight);

	void Render(D3DXMATRIX worldMatrix,D3DXMATRIX viewMatrix,D3DXMATRIX projectionMatrix, Vector3f eyePos, Light light, int lightType);
	~Tree(void);

private:
	Vector3f		mTreePosition;
	ID3D10Device	*md3dDevice;
	TextureLoader   *treeTexture;	
	
	std::vector<TreeSegment*>	treeSegmentList;

private:
	TexShader		*treeShader;
};



#endif