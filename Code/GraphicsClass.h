////////////////////////////////////////////////////////////////////////////////
// Filename: graphicsclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _GRAPHICSCLASS_H_
#define _GRAPHICSCLASS_H_


///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include <vector>

#include "d3dclass.h"
#include "GameCamera.h"
#include "inputclass.h"
#include "Terrain.h"
#include "TerrainGenerator.h"
#include "Light.h"
#include "cpuclass.h"
#include "textclass.h"
#include "TexShader.h"
#include "GameTimer.h"
#include "Tree.h"
#include "OrthoWindow.h"
#include "RenderTexture.h"
#include "Frustum.h"
#include "BoundingBox.h"
#include "Water.h"
#include "Skydome.h"
#include "Sun.h"
#include "BlurEffect.h"

using namespace std;

/////////////
// GLOBALS //
/////////////
const bool FULL_SCREEN = false;
const bool VSYNC_ENABLED = true;
const float SCREEN_DEPTH = 1000.0f;
const float SCREEN_NEAR = 0.1f;


////////////////////////////////////////////////////////////////////////////////
// Class name: GraphicsClass
////////////////////////////////////////////////////////////////////////////////
class GraphicsClass
{
public:
	GraphicsClass();
	GraphicsClass(const GraphicsClass&);
	~GraphicsClass();

	bool Initialize(int, int, HWND, HINSTANCE);
	void InitLights();
	void InitCameras();
	void InitShaders(HWND hwnd);
	void InitTerrain(HWND hwnd);
	void InitMisc(HWND hwnd);
	void Shutdown();
	bool Frame();

private:
	bool RenderScene();
	bool BlurRender();
	bool RenderGUI();
	void RenderWater();
	void RenderSky();
	void Update(float dt);
	void UpdateCamera(float dt);
	void MousePick(int x, int y);

	void ComputeRayFromMouse(int x, int y, Vector3f *outRayOrigin, Vector3f *outRayDir);
private:
	D3DClass*		mD3D;
	InputClass*		mInput;

	CpuClass		*mCpu;
	TextClass		*mText;

	GameTimer		mTimer;
	Light			mLight;

	Terrain			*mTerrain;
	Water			*mWater;
	Skydome			*mSkydome;
	Sun				*mSun;

	GameCamera		*mCamera;
	Frustum			*mFrustum;

	BlurEffect		*mBlurEffect;

	OrthoWindow *mFullScreenWindow;

	D3DXMATRIX mViewMatrix, mProjectionMatrix, mWorldMatrix, mOrthoMatrix;	
	Vector3f mCamPos;

	int mScreenWidth,mScreenHeight;

	HWND mHwnd;
private:
	//collections 
	vector<IShader*>		mShaderCollection;
	vector<Tree*>			mTreeCollection;
	vector<BaseGameObject*> mObjectCollection;
};

#endif