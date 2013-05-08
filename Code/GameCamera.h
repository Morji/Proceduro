/***************************************************************
GameCamera Class: Provides a simple camera to be used for any
3D DirectX application. 

Author: Valentin Hinov
Date: 13/03/2013
Version: 1.0

Exposes: GameCamera
**************************************************************/

#ifndef _GAMECAMERA_H_
#define _GAMECAMERA_H_

#include "d3dUtil.h"

class GameCamera
{
public:
	GameCamera(bool willBeAttached = false);

	~GameCamera(void);

	void SetPosition(D3DXVECTOR3 *pPosition);
	void SetRotation(D3DXVECTOR3 *pRotation);
	void SetUp(D3DXVECTOR3 *pUp);

	void GetPosition(D3DXVECTOR3 *outCamPos);
	D3DXVECTOR3 GetLookAtTarget();
	void GetRotation(D3DXVECTOR3 *outCamRot);
	D3DXVECTOR3	GetPivotPoint();

	void Render();
	void MoveYawPitch(float yaw, float pitch);
	void GetViewMatrix(D3DXMATRIX *outViewMatrix);
	void ModifyCamMovement(float amount);
	void SetPivotPoint(D3DXVECTOR3 &pos);	//for a third person camera - decide the distance from the character

	float moveLeftRight,moveBackForward;	
	float camMoveFactor;

private:	
	D3DXVECTOR3 mPosition;
	bool		mAttached;

	float yaw, pitch;
	D3DXVECTOR3	pivotPoint;

	D3DXVECTOR3 lookAt;
	D3DXVECTOR3 up;
	D3DXVECTOR3 right;
	D3DXVECTOR3 defaultForward;
	D3DXVECTOR3 defaultRight;
	D3DXVECTOR3 forward;

	D3DXMATRIX  mViewMatrix;
};


#endif