#include "TreeSegment.h"

TreeSegment::TreeSegment(){
	mParentSegment = 0;
	mGrowthCount = 0;
	mRadius = 1.0f;
	mHeight = 1.0f;
	mGrowthDirection = Vector3f(0,0,0);
}

TreeSegment::~TreeSegment(){
	mParentSegment = nullptr;
}

void TreeSegment::IncrementGrowthDir(Vector3f growthDir){
	mGrowthDirection += growthDir;
	mGrowthCount++;
}

void TreeSegment::ResetGrowth(){
	mGrowthCount = 0;
	mGrowthDirection = Vector3f(0,0,0);
}

int TreeSegment::GetGrowthCount(){
	return mGrowthCount;
}

CylinderObject *TreeSegment::GetGameObject(){
	return &mObject;
}

TreeSegment	*TreeSegment::GetParent(){
	return mParentSegment;
}

void TreeSegment::GetPosition(Vector3f &pos){
	pos = mPosition;
}

void TreeSegment::GetGrowthDirection(Vector3f &growthDir){
	growthDir = mGrowthDirection;
}


void TreeSegment::IncrementRadius(float amount){
	mRadius += amount;
	mObject.SetRadius(mRadius);
}

void TreeSegment::SetRadius(float radius){
	mRadius = radius;
	mObject.SetRadius(mRadius);
}

void TreeSegment::SetPosition(Vector3f position){
	mPosition = position;
	mObject.pos = position;
}

void TreeSegment::SetHeight(float height){
	mHeight = height;
	mObject.SetHeight(height);
}

void TreeSegment::SetParent(TreeSegment *parentSegment){
	mParentSegment = parentSegment;
}
