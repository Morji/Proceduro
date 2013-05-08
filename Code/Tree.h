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

#include "ID3DObject.h"
#include "TexShader.h"
#include "TreeShader.h"
#include "TreeSegment.h"
#include "TextureLoader.h"
#include "BaseGameObject.h"
#include "Renderer.h"
#include "Transform.h"
#include "BoundingBox.h"

#define MAX_LOD		9
#define MIN_LOD		4
#define MAX_DIST    180.0f
#define MIN_DIST	80.0f

class Tree : public BaseGameObject{
public:
	Tree(Vector3f position = Vector3f(0,0,0));

	bool Initialize(ID3D10Device* device, HWND hwnd);
	bool GenerateTreeSpaceExploration(ID3D10Device* device, float minGrowthSize, float startRadius, float maxHeight);
	void Render(ID3DObject *d3dObject, D3DXMATRIX viewMatrix, Vector3f eyePos, Light light, int lightType);

	~Tree(void);
private:
	struct TreeVertex{
		Vector3f pos;
		Vector3f parent;
		float	 radius;
		float	 parentRadius;
	};

private:
	TextureLoader   *mTreeTexture;	
	TreeShader		*mTreeShader;

private:
	//Components
	Renderer		*mRenderer;
	Transform		*mTransform;
	BoundingBox		*mBoundingBox;
};



#endif