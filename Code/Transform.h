/***************************************************************
Transform Class: Provides a transform component to game objects.

Author: Valentin Hinov
Date: 14/03/2013
Version: 1.0

Exposes: Transform
**************************************************************/

#ifndef _H_TRANSFORM_H
#define _H_TRANSFORM_H

#include "IComponent.h"
#include "Vertex.h"

class Transform : public IComponent{
public:
	Transform(): position(1,1,1),rotation(0,0,0),scale(1,1,1){

	}

	Vector3f	position;
	Vector3f	rotation;
	Vector3f	scale;

	void		GetTransformMatrix(D3DXMATRIX &matrix);

	~Transform(){}	
};

#endif