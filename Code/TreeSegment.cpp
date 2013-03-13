#include "TreeSegment.h"

TreeSegment::TreeSegment(){
	mParentSegment = 0;
	mGrowthCount = 0;
	mRadius = 1.0f;
	mHeight = 1.0f;
	mChildCount = 0;
	mGrowthDirection = Vector3f(0,0,0);
}

TreeSegment::~TreeSegment(){
	mParentSegment = nullptr;
}

void TreeSegment::AddChildCount(){
	mChildCount++;
	if (mParentSegment){
		mParentSegment->AddChildCount();
	}
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

int TreeSegment::GetChildCount(){
	return mChildCount;
}

TreeSegment	*TreeSegment::GetParent(){
	return mParentSegment;
}

float TreeSegment::GetRadius(){
	return mRadius;
}

void TreeSegment::GetPosition(Vector3f &pos){
	pos = mPosition;
}

void TreeSegment::GetGrowthDirection(Vector3f &growthDir){
	growthDir = mGrowthDirection;
}


void TreeSegment::IncrementRadius(float amount){
	mRadius += amount;
	if (mParentSegment){
		mParentSegment->IncrementRadius(amount);
	}
}

void TreeSegment::SetRadius(float radius){
	mRadius = radius;
}

void TreeSegment::SetPosition(Vector3f position){
	mPosition = position;
}

void TreeSegment::SetHeight(float height){
	mHeight = height;
}

void TreeSegment::SetParent(TreeSegment *parentSegment){
	mParentSegment = parentSegment;
	mParentSegment->AddChildCount();
}
