/***************************************************************
BoundingBox Class: Provides a bounding box component to be used
as needed: for collision detection, culling, etc...

Author: Valentin Hinov
Date: 13/03/2013
Version: 1.0

Exposes: BoundingBox
**************************************************************/

#ifndef _H_BOUNDINGBOX_H
#define _H_BOUNDINGBOX_H

#include <vector>
#include "Component.h"
#include "Vertex.h"

class BoundingBox : public Component{
public:
	BoundingBox(BaseGameObject *parent);

	// Get the bounds in local space
	void GetBounds(Vector3f *boundsMin, Vector3f *boundsMax);

	// Get the bounds in world space
	void GetBoundsWorldSpace(Vector3f &boundsMin, Vector3f &boundsMax);

	// Calculates a bounding box from all of the points that define a model
	bool CalculateBounds(Vector3f *firstVertex, int vertexCount, int stride);

	// Perform a ray intersection on this bounding box - specify if you want it in world of local space, default is local
	bool IntersectsRay(Vector3f rayOrigin, Vector3f rayDir, bool localSpace = true)const;

	// Perform a ray intersection on this bounding box - additionally supply modified ray data in local space
	bool IntersectsRay(Vector3f rayOrigin, Vector3f rayDir, Vector3f *outLocalRayOrigin, Vector3f *outLocalRayDir)const;

	~BoundingBox();
private:
	Vector3f	mBoundsMin;
	Vector3f	mBoundsMax;

	void TransformRayToLocalSpace(Vector3f *rayOrigin, Vector3f *rayDir)const;
};

#endif