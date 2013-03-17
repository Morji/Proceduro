/*************************************************************
Furstum Class: Used to instantiate a frustum based on a view
and projection matrices. Provides function to check if
geometrical shapes lie within this frustum

Author: Valentin Hinov
Based on: http://www.rastertek.com/dx10tut16.html
Date: 13/03/2013
Version: 1.0

Exposes: Frustum
**************************************************************/
#ifndef _FRUSTUM_H_
#define _FRUSTUM_H_


//////////////
// INCLUDES //
//////////////
#include <d3dx10math.h>
#include "BoundingBox.h"
#include "Vertex.h"


////////////////////////////////////////////////////////////////////////////////
// Class name: Frustum
////////////////////////////////////////////////////////////////////////////////
class Frustum
{
public:
	Frustum();
	Frustum(const Frustum&);
	~Frustum();

	void ConstructFrustum(float, D3DXMATRIX, D3DXMATRIX);

	bool CheckPoint(float, float, float);
	bool CheckCube(float, float, float, float);
	bool CheckSphere(float, float, float, float);
	bool CheckRectangle(float, float, float, float, float, float);

	bool CheckBoundingBox(BoundingBox *boundingBox);

private:
	D3DXPLANE m_planes[6];
};

#endif