/*************************************************************
Water Class: Encapsulates a flat plane that can be animated
using the wave equation

Author: Valentin Hinov
Date: 19/03/2013
Version: 1.0

Exposes: Water
**************************************************************/

#include <iostream>
#include "BaseGameObject.h"
#include "WaterShader.h"
#include "Renderer.h"
#include "RenderTexture.h"
#include "BoundingBox.h"
#include "Transform.h"
#include "ID3DObject.h"
#include <cmath>

#ifndef _H_WATER_H
#define _H_WATER_H

#define CELLSPACING		1.0f
#define GRIDSIZE		100
#define WAVE_SPEED		3.0f
#define TIME_STEP		0.0007f
#define DAMP_FACTOR		0.1f
#define COURANT_NUMBER  0.03f

#define WATER_BLUE_COLOR	Color(0.2f,0.0f,0.98f,1.0f)

const float  SPACING = CELLSPACING/GRIDSIZE;

class Water : public BaseGameObject{
public:
	Water(Vector3f position);

	bool Initialize(ID3D10Device* device, HWND hwnd);

	void Render(ID3DObject* d3dObject, D3DXMATRIX viewMatrix, Vector3f eyePos, Light light, int lightType);

	bool CanHandlePicking(Vector3f rayOrigin, Vector3f rayDir);

	void SetSimulationState(bool state);
	bool GetSimulationState();
	~Water(void);
	
private:
	void RunSimulation(ID3D10Device* device);	
	bool simulateWave;
	bool simulateHeat;

	bool CheckTriangleIntersection(int index,Vector3f *rayOrigin,Vector3f *rayDir, Vector3f *outIntersectionPoint);

	WaterShader	*mWaterShader;

	struct WaterVertex{
		Vector3f pos;
		Vector3f normal;
		Color color;
	};
	
	float GetHeight(int i, int j);
	float GetHeat(int i, int j);
	void  ComputeWaveEqn(int i, int j);
	void  ComputeHeatEqn(int i, int j);

	WaterVertex *vertices;
	DWORD		*indices; 
	float		*prevStepHeight;
	float		*nextStepHeight;

	float		*currHeat;
	float		*nextStepHeat;
	//RenderTexture *mCurrHeightsTexture;
	//RenderTexture *mPrevHeightsTexture;
	//RenderTexture *mNextHeightsTexture;

// components
private:
	Renderer	*mRenderer;
	Transform	*mTransform;
	BoundingBox *mBoundingBox;
};

#endif