/*************************************************************
BlurEffect Class: Used to perform a blur post processing
effect on a target RTT

Author: Valentin Hinov
Date: 08/05/2013
Version: 1.0

Exposes: BlurEffect
**************************************************************/

#include "OrthoTextureShader.h"
#include "OrthoWindow.h"
#include "BlurShader.h"
#include "RenderTexture.h"
#include "ID3DObject.h"
#include <vector>

#ifndef _BLUREFFECT_H
#define _BLUREFFECT_H

class BlurEffect
{
public:
	BlurEffect();
	~BlurEffect(void);

	bool Initialize(ID3DObject *d3dObject, HWND hwnd);

	// Must be called to prepare the blur renderer for the current target scene to blur
	void PrepareForSceneRender();
	// Called after the target scene is renderer
	void SceneRenderFinished();

	void RenderToWindow(OrthoWindow *orthoWindow);

private:
	void DownSampleTexture();
	void RenderBlurToTexture();
	void UpSampleTexture();
	void Render2DTextureScene();

private:
	RenderTexture *mRenderTexture, *mDownSampleTexture, *mHorizontalBlurTexture, *mVerticalBlurTexture, *mUpSampleTexture;
	
	OrthoTextureShader		*mOrthoTexShader;
	BlurShader				*mBlurShader;

	ID3DObject *pd3dObject;
	OrthoWindow *pOrthoWindow;

	std::vector<RenderTexture*> textureCollection;
	
};

#endif