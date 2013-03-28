/*************************************************************
TerrainNode Class: Extends the renderer component into a quad
tree node for the terrain so rendering the terrain can be
split into sections

Author: Valentin Hinov
Date: 18/03/2013
Version: 1.0

Exposes: TerrainNode
**************************************************************/

#ifndef _H_TERRAINNODE_H
#define _H_TERRAINNODE_H

#include "Renderer.h"
class TerrainNode : public Renderer{
public:
	TerrainNode(void):Renderer(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST),positionX(0),positionZ(0),diameter(0),triangleCount(0){
		for (int i = 0; i < 4; i++){
			childNodes[i] = 0;
		}
	}

	~TerrainNode(void);

	float positionX;
	float positionZ;
	float diameter;
	int   triangleCount;
	TerrainNode *childNodes[4];
};

#endif