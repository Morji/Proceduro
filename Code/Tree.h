/*************************************************************
Tree Class: Used to instantiate and create a tree object via 
procedural methods.

Author: Valentin Hinov
Date: 03/03/2013
Version: 1.1

Exposes: Tree
Supported methods: Space Exploration
**************************************************************/

#ifndef _H_TREE_H
#define _H_TREE_H

#include <vector>

#include "CylinderObject.h"
#include "TexShader.h"
#include "TreeShader.h"
#include "TreeSegment.h"

class Tree{
public:
	Tree(ID3D10Device *d3dDevice, HWND hwnd, Vector3f position);

	bool GenerateTreeSpaceExploration(float minGrowthSize, float startRadius, float maxHeight);

	void Render(D3DXMATRIX worldMatrix,D3DXMATRIX viewMatrix,D3DXMATRIX projectionMatrix, Vector3f eyePos, Light light, int lightType);
	~Tree(void);

private:

	struct TreeVertex{
		Vector3f pos;
		Vector3f parent;
		float	 radius;
		float	 parentRadius;
	};

	void RenderTreeBuffers();
	bool InitializeBuffers(DWORD* indices, TreeVertex* vertices);

private:
	Vector3f		mTreePosition;
	ID3D10Device	*md3dDevice;
	TextureLoader   *treeTexture;	
	
	std::vector<TreeSegment*>	treeSegmentList;

private:
	TexShader		*treeShader;
	TreeShader		*newTreeShader;

	ID3D10Buffer	*mVB;
	ID3D10Buffer	*mIB;
	unsigned int	stride;
	unsigned int	offset;
	unsigned int	mVertexCount;
	unsigned int	mIndexCount;
};



#endif