/*************************************************************
Sun Class: Encapsulates a Sun object which is a sphere
that moves with the camera and also rotates around a circle

Author: Valentin Hinov
Date: 09/04/2013
Version: 1.0

Requires: BaseGameObject.h, Renderer.h, SkyShader.h
Exposes: Sun
**************************************************************/

#ifndef _H_SUN_H
#define _H_SUN_H

#include "BaseGameObject.h"
#include "Renderer.h"
#include "ColorShader.h"
#include "Transform.h"
#include "ID3DObject.h"

const float DAY_CYCLE_IN_MINUTES = 0.2f; // How long the day lasts as part of a minute

const float SECOND = 1;
const float MINUTE = 60*SECOND;
const float HOUR = 60*MINUTE;
const float DAY = 24*HOUR;

const float DAY_LENGTH_IN_MINUTES = DAY_CYCLE_IN_MINUTES*MINUTE;

const Color MAX_AMBIENT_LIGHT = Color(0.5f,0.5f,0.5f,1.0f);
const Color MIN_AMBIENT_LIGHT = Color(0.0f,0.0f,0.0f,1.0f);

const float DEGREES_PER_SECOND = 360.0f/DAY;

class Sun :	public BaseGameObject{
public:
	Sun(void);
	~Sun(void);

	bool Initialize(ID3D10Device* device, HWND hwnd);
	void Render(ID3DObject* d3dObject, D3DXMATRIX viewMatrix);
	void Update(float dt);
	void SetOrbitPoint(Vector3f *orbitPoint);
	void SetOrbitDistance(float distance);
	
	void GetLightDirection(Vector3f *dir);
	void GetAmbientLight(Color *ambient);

// Components
private:
	Renderer	*mSunRenderer;
	Transform	*mTransform;

private:
	ColorShader	*mSunShader;	
	Color		mColor;

private:
	float		angle;
	float		mOrbitDistance;
	float		mDegreeRotation;
	float		mRadianRotation;
	float		mTimeOfDay;

	Vector3f	orbitPoint;
};

#endif