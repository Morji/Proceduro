#include "Skydome.h"
#include "GeometryBuilder.h"

Skydome::Skydome(void): mSkyRenderer(nullptr), mSkyShader(nullptr), day(false){
	AddComponent(mSkyRenderer, RENDERER);
	mTransform = new Transform(this);
	AddComponent(mTransform,TRANSFORM);

	// Set the color at the top of the sky dome.
	mApexColor = Color(0.0f, 0.15f, 0.66f, 1.0f);

	// Set the color at the center of the sky dome.
	mCenterColor =  Color(0.31f, 0.58f, 0.66f, 1.0f);
}


Skydome::~Skydome(void){
	if (mSkyShader){
		delete mSkyShader;
		mSkyShader = nullptr;
	}
}

void Skydome::Render(ID3DObject* d3dObject, D3DXMATRIX viewMatrix){
	mSkyRenderer->RenderBuffers(d3dObject->GetDevice());

	D3DXMATRIX mObjMatrix;
	d3dObject->GetWorldMatrix(mObjMatrix);
	mTransform->GetTransformMatrix(mObjMatrix);

	mSkyShader->Render(d3dObject->GetDevice(),mSkyRenderer->GetIndexCount(),mObjMatrix,viewMatrix,*d3dObject->GetProjectionMatrix(),mApexColor,mCenterColor);
}

void Skydome::Update(float dt){
	if (day){
		mApexColor.b -= dt/10.0f;
		mCenterColor.b -= dt/10.0f;
		if (mApexColor.b <= 0.5f){
			day = false;
			mApexColor.b = mCenterColor.b = 0.5f;
		}
	}
	else{
		mApexColor.b += dt/10.0f;
		mCenterColor.b += dt/10.0f;
		if (mApexColor.b >= 0.9f){
			day = true;
			mApexColor.b = mCenterColor.b = 0.9f;
		}
	}
}

bool Skydome::Initialize(ID3D10Device* device, HWND hwnd){
	mSkyRenderer = new Renderer(this,D3D10_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	bool result = BuildSphere(20,20,device,mSkyRenderer);

	if (!result){
		MessageBox(hwnd, L"Error", L"Error initializing sky sphere", MB_OK);
		return false;
	}

	mSkyShader = new SkyShader();
	result = mSkyShader->Initialize(device,hwnd);

	if (!result){
		return false;
	}

	return true;
}