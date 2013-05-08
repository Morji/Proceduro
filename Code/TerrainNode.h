/*************************************************************
TerrainNode Class: Extends the renderer component into a quad
tree node for the terrain so rendering the terrain can be
split into sections. Provides a method for drawing the
bounds the node encapsulates for debugging purposes.

Author: Valentin Hinov
Date: 18/03/2013
Version: 1.2

Exposes: TerrainNode
**************************************************************/

#ifndef _H_TERRAINNODE_H
#define _H_TERRAINNODE_H

#include "Renderer.h"
#include "ID3DObject.h"
#include "ColorShader.h"

class TerrainNode{
public:
	TerrainNode(void):positionX(0),positionZ(0),diameter(0){
		for (int i = 0; i < 4; i++){
			childNodes[i] = 0;
		}
		mBoundsRenderer = nullptr;
		mColorShader = nullptr;
		nodeRenderer = new Renderer(nullptr,D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	}

	~TerrainNode(void);

	void DrawBounds(ID3DObject *d3dObject, D3DXMATRIX viewMatrix, float maxHeight);

	float positionX;
	float positionZ;
	float diameter;
	TerrainNode *childNodes[4];
	Renderer *nodeRenderer;

private:
	Renderer	*mBoundsRenderer;
	ColorShader *mColorShader;
};

#endif