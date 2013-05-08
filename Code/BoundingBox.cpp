#include "BoundingBox.h"
#include "Transform.h"
#include "BaseGameObject.h"

BoundingBox::BoundingBox(BaseGameObject *parent): Component(parent){

}

BoundingBox::~BoundingBox(){
	
}

void BoundingBox::GetBounds(Vector3f *boundsMin, Vector3f *boundsMax){
	boundsMin = &mBoundsMin;
	boundsMax = &mBoundsMax;
}

void BoundingBox::GetBoundsWorldSpace(Vector3f &boundsMin, Vector3f &boundsMax){
	Transform *pParentTransform = (Transform*)pParent->GetComponent(TRANSFORM);
	boundsMin = (mBoundsMin + pParentTransform->position);
	boundsMax = (mBoundsMax + pParentTransform->position);
}

bool BoundingBox::IntersectsRay(Vector3f rayOrigin, Vector3f rayDir, bool localSpace)const{
	//if local space is specified, transform ray to local space
	if (localSpace){
		TransformRayToLocalSpace(&rayOrigin,&rayDir);
	}
	return D3DXBoxBoundProbe(&mBoundsMin,&mBoundsMax,&rayOrigin,&rayDir) != 0;
}

bool BoundingBox::IntersectsRay(Vector3f rayOrigin, Vector3f rayDir, Vector3f *outLocalRayOrigin, Vector3f *outLocalRayDir)const{
	TransformRayToLocalSpace(&rayOrigin,&rayDir);

	bool hit = D3DXBoxBoundProbe(&mBoundsMin,&mBoundsMax,&rayOrigin,&rayDir) != 0;
	if (hit){
		*outLocalRayOrigin = rayOrigin;
		*outLocalRayDir = rayDir;
	}
	else{
		outLocalRayOrigin = nullptr;
		outLocalRayDir = nullptr;
	}
	return hit;
}

void BoundingBox::TransformRayToLocalSpace(Vector3f *rayOrigin, Vector3f *rayDir)const{
	Transform *pParentTransform = (Transform*)pParent->GetComponent(TRANSFORM);
	D3DXMATRIX inverseW;
	D3DXMATRIX mObjMatrix;
	pParentTransform->GetTransformMatrix(mObjMatrix);	
	D3DXMatrixInverse(&inverseW, 0, &mObjMatrix);
	D3DXVec3TransformCoord(rayOrigin, rayOrigin, &inverseW);
	D3DXVec3TransformNormal(rayDir, rayDir, &inverseW);
}

bool BoundingBox::CalculateBounds(Vector3f *firstVertex, int vertexCount, int stride){
	HRESULT hr = D3DXComputeBoundingBox(firstVertex,vertexCount,stride,&mBoundsMin,&mBoundsMax);
	if (FAILED(hr)){
		return false;
	}
	return true;
}