/*************************************************************
TreeSegment Class: Used to handle generation of the tree for 
space exploration

Author: Valentin Hinov
Date: 21/02/2013
Version: 1.0

Exposes: TreeSegment
**************************************************************/

#ifndef _H_TREESEGMENT_H
#define _H_TREESEGMENT_H

#include "Vertex.h"

#include "CylinderObject.h"

//TreeSegment class 
class TreeSegment{
public:	

	TreeSegment(void);
	~TreeSegment(void);

	void			IncrementGrowthDir(Vector3f growthDir);
	void			ResetGrowth();

	void			SetPosition(Vector3f position);
	void			SetParent(TreeSegment *parentSegment);
	void			SetHeight(float height);
	void			SetRadius(float radius);
	void			IncrementRadius(float amount);

	int				GetGrowthCount();
	CylinderObject	*GetGameObject();
	TreeSegment		*GetParent();
	void			GetPosition(Vector3f &pos);
	void			GetGrowthDirection(Vector3f &growthDir);

private:
	Vector3f		mPosition;
	Vector3f		mGrowthDirection;
	int				mGrowthCount;
	float			mRadius;
	float			mHeight;

	TreeSegment		*mParentSegment;
	CylinderObject	mObject;
};


#endif