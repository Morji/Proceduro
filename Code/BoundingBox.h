/***************************************************************
BoundingBox Class: Provides a bounding box component to be used
as needed: for collision detection, culling, etc...

Author: Valentin Hinov
Date: 13/03/2013
Version: 1.0

Exposes: IComponent
**************************************************************/

#ifndef _H_BOUNDINGBOX_H
#define _H_BOUNDINGBOX_H

#include <vector>
#include "IComponent.h"
#include "Vertex.h"
#include "Transform.h"

class BoundingBox : public IComponent{
public:
	BoundingBox(Transform *parentTransform);

	// Get the bounds - center is returned in world space
	void GetBounds(Vector3f &center, Vector3f &size);

	// Calculates a bounding box from all of the points that define a model
	void CalculateBounds(std::vector<Vector3f*> *vertices);

	~BoundingBox();
private:
	Transform	*mParentTransform;
	Vector3f	mCenter;
};

#endif