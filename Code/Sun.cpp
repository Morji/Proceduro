#include "Sun.h"
#include "GeometryBuilder.h"
#include <iostream>

Sun::Sun(void):mSunRenderer(nullptr), angle(0.0f), mOrbitDistance(30.0f), mDegreeRotation(0.0f), mTimeOfDay(0.0f)
{
	AddComponent(mSunRenderer,RENDERER);
	mTransform = new Transform(this);
	AddComponent(mTransform,TRANSFORM);
	mColor = Color(1.0f,1.0f,1.0f,1.0f);

	mDegreeRotation = DEGREES_PER_SECOND * DAY / (DAY_CYCLE_IN_MINUTES * MINUTE);
	mRadianRotation = mDegreeRotation * PI/180.0f;
}


Sun::~Sun(void){
	if (mSunShader){
		delete mSunShader;
		mSunShader = nullptr;
	}
}

bool Sun::Initialize(ID3D10Device* device, HWND hwnd){
	mSunRenderer = new Renderer(this,D3D10_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	bool result = BuildColoredSphere(10,10,device,mSunRenderer,mColor);

	if (!result){
		MessageBox(hwnd, L"Error", L"Error initializing sun sphere", MB_OK);
		return false;
	}

	mSunShader = new ColorShader();
	result = mSunShader->Initialize(device,hwnd);

	if (!result){
		MessageBox(hwnd, L"Error", L"Error initializing sun shader", MB_OK);
		return false;
	}

	return true;
}

void Sun::Render(ID3DObject* d3dObject, D3DXMATRIX viewMatrix){
	mSunRenderer->RenderBuffers(d3dObject->GetDevice());

	D3DXMATRIX mObjMatrix;
	d3dObject->GetWorldMatrix(mObjMatrix);
	mTransform->GetTransformMatrix(mObjMatrix);

	mSunShader->Render(d3dObject->GetDevice(),mSunRenderer->GetIndexCount(),mObjMatrix,viewMatrix,*d3dObject->GetProjectionMatrix());
}

void Sun::Update(float dt){
	angle+=dt;
	mTimeOfDay+=dt;
	float angleDivSeconds = angle*mRadianRotation;
	mTransform->position = orbitPoint;
	mTransform->position.x += mOrbitDistance*cosf(angleDivSeconds);
	mTransform->position.y += mOrbitDistance*sinf(angleDivSeconds);
	
	if (mTimeOfDay > DAY_LENGTH_IN_MINUTES){
		// reset time of day counter
		mTimeOfDay -= DAY_LENGTH_IN_MINUTES;		
	}
}

void Sun::GetAmbientLight(Color *ambient){
	float lerpLight = mTimeOfDay/DAY_LENGTH_IN_MINUTES*2;

	if (lerpLight > 1){
		lerpLight = 1 - (lerpLight - 1);
	}

	*ambient = lerpLight*MAX_AMBIENT_LIGHT + (1-lerpLight)*MIN_AMBIENT_LIGHT;

	//std::cout << "Ambient light: " << ambient->r << std::endl;
}

void Sun::GetLightDirection(Vector3f *dir){
	Vector3f lightDir = orbitPoint - mTransform->position;
	D3DXVec3Normalize(dir,&lightDir);
	float angleDivSeconds = angle*mRadianRotation;
	dir->x = -cos(angleDivSeconds);
	dir->y = -sin(angleDivSeconds);
	dir->z = -cos(angleDivSeconds);
}

void Sun::SetOrbitDistance(float distance){
	mOrbitDistance = distance;
}

void Sun::SetOrbitPoint(Vector3f *orbitPoint){
	this->orbitPoint = *orbitPoint;
}