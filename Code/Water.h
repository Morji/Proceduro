/*************************************************************
Water Class: Encapsulates a flat plane that can be animated
using the wave equation

Author: Valentin Hinov
Date: 19/03/2013
Version: 1.0

Exposes: Water
**************************************************************/

#ifndef _H_WATER_H
#define _H_WATER_H

#include <iostream>
#include "BaseGameObject.h"
#include "WaterShader.h"
#include "Renderer.h"
#include <cmath>

#define CELLSPACING		1.0f
#define GRIDSIZE		100
#define WAVE_SPEED		3.0f
#define TIME_STEP		0.0007f
#define DAMP_FACTOR		0.1f


const float  SPACING = CELLSPACING/GRIDSIZE;

class Water : public BaseGameObject{
public:
	Water(Vector3f position);

	bool Initialize(ID3D10Device* device, HWND hwnd);

	void Render(ID3D10Device* device,D3DXMATRIX worldMatrix,D3DXMATRIX viewMatrix,D3DXMATRIX projectionMatrix, Vector3f eyePos, Light light, int lightType);

	void HandlePicking(Vector3f rayOrigin, Vector3f rayDir);

	void SetSimulationState(bool state);
	bool GetSimulationState();
	~Water(void);
	
private:
	void RunSimulation(ID3D10Device* device);	
	bool simulate;

	bool CheckTriangleIntersection(int index,Vector3f &rayOrigin,Vector3f &rayDir, Vector3f &intersectionPoint);

	WaterShader	*mWaterShader;

	struct WaterVertex{
		Vector3f pos;
		Vector3f normal;
	};

	float GetHeight(int i, int j);
	void  ComputeWaveEqn(int i, int j);

	WaterVertex *vertices;
	DWORD		*indices; 
	float		*prevStepHeight;
	float		*nextStepHeight;

	Renderer	*mRenderer;
	Vector3f	*boundsMin;
	Vector3f	*boundsMax;
};

#endif