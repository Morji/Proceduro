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

#include "ValMath.h"
#include "TexShader.h"
#include "TreeShader.h"
#include "TreeSegment.h"
#include "BoundingBox.h"
#include "TextureLoader.h"
#include "BaseGameObject.h"
#include "Renderer.h"

#define MAX_LOD		9
#define MIN_LOD		4
#define MAX_DIST    180.0f
#define MIN_DIST	80.0f

class Tree : public BaseGameObject{
public:
	Tree(Vector3f position);

	bool Initialize(ID3D10Device* device, HWND hwnd);
	bool GenerateTreeSpaceExploration(float minGrowthSize, float startRadius, float maxHeight);
	void Render(D3DXMATRIX worldMatrix,D3DXMATRIX viewMatrix,D3DXMATRIX projectionMatrix, Vector3f eyePos, Light light, int lightType);

	BoundingBox		*GetBoundingBox();

	~Tree(void);
private:
	struct TreeVertex{
		Vector3f pos;
		Vector3f parent;
		float	 radius;
		float	 parentRadius;
	};

private:
	ID3D10Device	*md3dDevice;
	TextureLoader   *treeTexture;	
	
	TreeShader		*mTreeShader;

private:
	//Components
	BoundingBox		*mBoundingBox;
	Renderer		*mRenderer;
};



#endif