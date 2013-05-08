#include "GameCamera.h"


GameCamera::GameCamera(bool willBemAttached)
{
	mPosition = D3DXVECTOR3(0.0f,0.0f,0.0f);
	right = D3DXVECTOR3(1.0f,0.0f,0.0f);
	forward = D3DXVECTOR3(0.0f,0.0f,1.0f);
	defaultRight = D3DXVECTOR3(1.0f,0.0f,0.0f);
	defaultForward = D3DXVECTOR3(0.0f,0.0f,1.0f);

	moveLeftRight = moveBackForward = yaw = pitch = 0.0f;
	camMoveFactor = 15.0f;
	mAttached = willBemAttached;
}

GameCamera::~GameCamera(void)
{
}

void GameCamera::SetPivotPoint(D3DXVECTOR3 &pos){
	pivotPoint = pos;
}

void GameCamera::MoveYawPitch(float yaw, float pitch){	
	this->yaw += yaw;
	this->pitch += pitch;
}

//The Render function uses the mPosition and mRotation of the camera to build and update the view matrix.
void GameCamera::Render()
{
	D3DXMATRIX mRotationMatrix;	 

	// Setup the up vector.
	up = D3DXVECTOR3(0,1,0);

	// Setup where the camera is looking by default.
	lookAt = D3DXVECTOR3(0,0,1);

	// Create the mRotation matrix from the yaw, pitch, and roll values.
	D3DXMatrixRotationYawPitchRoll(&mRotationMatrix, yaw, pitch, 0);

	// Transform the lookAt and up vector by the mRotation matrix so the view is correctly rotated at the origin.
	D3DXVec3TransformCoord(&lookAt, &defaultForward, &mRotationMatrix);
	D3DXVec3Normalize(&lookAt,&lookAt);	

	D3DXMATRIX yawMatrix;
	D3DXMatrixRotationY(&yawMatrix, yaw);
	D3DXVec3TransformNormal(&right, &defaultRight, &yawMatrix);
	D3DXVec3TransformNormal(&up, &up, &mRotationMatrix);
	D3DXVec3TransformNormal(&forward, &defaultForward, &mRotationMatrix);

	D3DXVECTOR3 camPos;

	if (mAttached){
		D3DXVECTOR3 pivot;
		D3DXVec3TransformCoord(&pivot,&pivotPoint,&mRotationMatrix);
		camPos = mPosition + pivot;
	}
	else{		
		mPosition += moveLeftRight*right;
		mPosition += moveBackForward*forward;
		moveLeftRight = 0.0f;
		moveBackForward = 0.0f;
		camPos = mPosition;
	}

	// Translate the rotated camera mPosition to the location of the viewer.
	lookAt = camPos + lookAt;

	// Finally create the view matrix from the three updated vectors.
	D3DXMatrixLookAtLH(&mViewMatrix, &camPos, &lookAt, &up);
}

void GameCamera::ModifyCamMovement(float amount){
	if (!mAttached){
		camMoveFactor += amount;
		if (camMoveFactor < 0.0f){
			camMoveFactor = 0.0f;
		}
	}
}

///SETTERS
void GameCamera::SetPosition(D3DXVECTOR3 *pPosition){
	mPosition = *pPosition;
}

void GameCamera::SetRotation(D3DXVECTOR3 *pRotation){
	pitch = asinf(pRotation->y);
	yaw = asin(pRotation->x/cosf(pitch));
}

void GameCamera::SetUp(D3DXVECTOR3 *pUp){
	up = *pUp;
}

///GETTERS
void GameCamera::GetPosition(D3DXVECTOR3 *outCamPos){
	*outCamPos = mPosition;
}

void GameCamera::GetRotation(D3DXVECTOR3 *outCamRot){
	outCamRot->x = cosf(pitch) * sinf(yaw);
	outCamRot->y = sinf(pitch);
	outCamRot->z = cosf(pitch) * cosf(yaw);
	D3DXVec3Normalize(outCamRot,outCamRot);
}

D3DXVECTOR3 GameCamera::GetLookAtTarget(){
	return lookAt;
}

D3DXVECTOR3 GameCamera::GetPivotPoint(){
	return pivotPoint;
}

void GameCamera::GetViewMatrix(D3DXMATRIX *outViewMatrix){
	*outViewMatrix = mViewMatrix;
}