#ifndef _H_CYLINDEROBJECT_H
#define _H_CYLINDEROBJECT_H

#include "GameObject.h"

class CylinderObject : public GameObject{
public:

	CylinderObject(unsigned int numberOfSegments = 10, float radius = 1.0f, float height = 1.0f);

	void	SetRadius(float radius);
	void	SetHeight(float height);
	void	SetRotation(D3DXQUATERNION *quaternion);

	~CylinderObject(void);

private:
	bool SetupArraysAndInitBuffers();
	bool InitializeBuffers(DWORD* indices,  VertexNT* vertices);

	void setTrans(D3DXMATRIX worldMatrix);	

	unsigned int	mNumberOfSegments;
	float			mRadius;
	float			mHeight;

	D3DXMATRIX		mRotationMatrix;

};

#endif