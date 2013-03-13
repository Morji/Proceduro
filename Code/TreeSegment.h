/*************************************************************
TreeSegment Class: Used to handle generation of the tree for 
space exploration

Author: Valentin Hinov
Date: 03/03/2013
Version: 1.1

Exposes: TreeSegment
**************************************************************/

#ifndef _H_TREESEGMENT_H
#define _H_TREESEGMENT_H

#include "Vertex.h"

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
	int				GetChildCount();
	float			GetRadius();
	TreeSegment		*GetParent();
	void			GetPosition(Vector3f &pos);
	void			GetGrowthDirection(Vector3f &growthDir);

private:
	void			AddChildCount();

private:
	Vector3f		mPosition;
	Vector3f		mGrowthDirection;
	int				mGrowthCount;
	float			mRadius;
	float			mHeight;

	int				mChildCount;
	TreeSegment		*mParentSegment;
};


#endif