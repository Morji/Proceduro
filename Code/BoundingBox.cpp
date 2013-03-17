/***************************************************************
BoundingBox Class: Provides a bounding box component to be used
as needed: for collision detection, culling, etc...

Author: Valentin Hinov
Date: 13/03/2013
Version: 1.0

Exposes: IComponent
**************************************************************/

#include "BoundingBox.h"

BoundingBox::BoundingBox(Transform *parentTransform){
	mCenter = Vector3f(0,0,0);
	mParentTransform = parentTransform;
	
}

BoundingBox::~BoundingBox(){
	mParentTransform = nullptr;
}

void BoundingBox::GetBounds(Vector3f &center, Vector3f &size){
	center = mParentTransform->position + mCenter;
	size.x = mCenter.x * mParentTransform->scale.x;
	size.y = mCenter.y * mParentTransform->scale.y;
	size.z = mCenter.z * mParentTransform->scale.z;
}

void BoundingBox::CalculateBounds(std::vector<Vector3f*> *vertices){
	//find the furthest X, Y and Z from the center
	float xMax = 0.0f;
	float yMax = 0.0f;
	float zMax = 0.0f;
	float xMin = 0.0f;
	float yMin = 0.0f;
	float zMin = 0.0f;

	int numVerts = vertices->size();
	for (int i = 0; i < numVerts; i++){
		//compare x
		if ( vertices->at(i)->x > xMax){
			xMax = vertices->at(i)->x;
		}
		if ( vertices->at(i)->x < xMin){
			xMin = vertices->at(i)->x;
		}
		//compare y
		if ( vertices->at(i)->y > yMax){
			yMax = vertices->at(i)->y;
		}
		if ( vertices->at(i)->y < yMin){
			yMin = vertices->at(i)->y;
		}
		//compare z
		if ( vertices->at(i)->z > zMax){
			zMax = vertices->at(i)->z;
		}
		if ( vertices->at(i)->z < zMin){
			zMin = vertices->at(i)->z;
		}
	}
	//set center point
	float width = xMax-xMin;
	float height = yMax-yMin;
	float depth = zMax-zMin;
	mCenter = Vector3f(width/2.0f,height/2.0f,depth/2.0f);
}