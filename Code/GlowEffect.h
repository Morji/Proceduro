/*************************************************************
GlowEffect Class: Used to perform a glow post processing
effect on a target RTT

Author: Valentin Hinov
Date: 08/05/2013
Version: 1.0

Exposes: GlowEffect
**************************************************************/

#include "OrthoTextureShader.h"
#include "OrthoWindow.h"
#include "GlowShader.h"
#include "BlurShader.h"
#include "RetainBrightnessShader.h"
#include "RenderTexture.h"
#include "ID3DObject.h"
#include <vector>

#ifndef _GLOWEFFECT_H
#define _GLOWEFFECT_H

#define LUMINANCE_THRESHOLD 0.8f //how luminant? a pixel has to be in order to glow (0 is not bright at all, 1 is fully bright)
#define GLOW_STRENGTH		2.0f //how much the glow will get amplified by

class GlowEffect
{
public:
	GlowEffect();
	~GlowEffect(void);

	bool Initialize(ID3DObject *d3dObject, HWND hwnd);

	// Must be called to prepare the renderer for the current target scene to blur
	void PrepareForSceneRender();
	// Called after the target scene is renderer
	void SceneRenderFinished();

	void RenderToWindow(OrthoWindow *orthoWindow);

private:
	void DownSampleTexture();
	void RetainTextureBrightness();
	void RenderBlurToTexture();
	void UpSampleTexture();
	void Render2DTextureScene();

private:
	RenderTexture *mRenderTexture, *mDownSampleTexture, *mBrightnessRetainedTexture, *mHorizontalBlurTexture, *mVerticalBlurTexture, *mUpSampleTexture;
	
	OrthoTextureShader		*mOrthoTexShader;
	BlurShader				*mBlurShader;
	RetainBrightnessShader  *mRetainBrightnessShader;
	GlowShader				*mGlowShader;

	ID3DObject *pd3dObject;
	OrthoWindow *pOrthoWindow;

	std::vector<IShader*> mShaderCollection;
	std::vector<RenderTexture*> mTextureCollection;
	
};

#endif