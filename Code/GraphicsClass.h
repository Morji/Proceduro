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
#include "MultiTextureShader.h"
#include "GameTimer.h"
#include "Tree.h"
#include "OrthoWindow.h"
#include "HorizontalBlurShader.h"
#include "VerticalBlurShader.h"
#include "RenderTexture.h"
#include "OrthoTextureShader.h"

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
	bool Render();
	bool RenderScene();
	bool BlurRender();
	bool RenderGUI();
	void Update(float dt);
	void UpdateCamera(float dt);

	void RenderSceneToTexture();
	void DownSampleTexture();
	void RenderHorizontalBlurToTexture();
	void RenderVerticalBlurToTexture();
	void UpSampleTexture();
	void Render2DTextureScene();

private:
	D3DClass*		mD3D;
	InputClass*		mInput;

	CpuClass		*mCpu;
	TextClass		*mText;

	GameTimer		mTimer;
	Light			mLight;

	Tree			*mTree;

	Terrain			*mTerrain;

	GameCamera		*mCamera;

	MultiTextureShader		*mMultiTexShader;
	TexShader				*mRegularTexShader;
	OrthoTextureShader		*mOrthoTexShader;
	HorizontalBlurShader	*mHorizontalBlurShader;
	VerticalBlurShader		*mVerticalBlurShader;

	RenderTexture *m_RenderTexture, *m_DownSampleTexure, *m_HorizontalBlurTexture, *m_VerticalBlurTexture, *m_UpSampleTexure;
	OrthoWindow *mFullScreenWindow;

	D3DXMATRIX mViewMatrix, mProjectionMatrix, mWorldMatrix, mOrthoMatrix;

private:
	//collections 
	vector<IShader*>	shaderCollection;
};

#endif