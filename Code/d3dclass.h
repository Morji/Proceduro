////////////////////////////////////////////////////////////////////////////////
// Filename: d3dclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _D3DCLASS_H_
#define _D3DCLASS_H_


#include "ID3DObject.h"


////////////////////////////////////////////////////////////////////////////////
// Class name: D3DClass
////////////////////////////////////////////////////////////////////////////////
class D3DClass: public ID3DObject
{
public:
	D3DClass();
	D3DClass(const D3DClass&);
	~D3DClass();

	bool Initialize(int, int, bool, HWND, bool, float, float);
	void Shutdown();
	
	void BeginScene(float, float, float, float);
	void EndScene();

	ID3D10Device* GetDevice();
	D3D10_VIEWPORT	*GetViewport();

	D3DXMATRIX *GetProjectionMatrix();
	D3DXMATRIX *GetWorldMatrix();
	D3DXMATRIX *GetOrthoMatrix();
	void GetProjectionMatrix(D3DXMATRIX&);
	void GetWorldMatrix(D3DXMATRIX&);
	void GetOrthoMatrix(D3DXMATRIX&);

	void GetScreenDimensions(int *width, int *height);
	void GetScreenDepthInfo(float *nearVal, float *farVal);

	void SetBackBufferRenderTarget();
	void ResetViewport();

	void TurnZBufferOn();
	void TurnZBufferOff();
	void TurnOnAlphaBlending();
	void TurnOffAlphaBlending();

	void TurnWireframeOn();
	void TurnWireframeOff();

	void TurnCullingOn();
	void TurnCullingOff();

	bool Screenshot();

	void GetVideoCardInfo(char*, int&);

private:
	bool BuildRasterizers();
	bool BuildBlendStates();
	bool BuildDepthStencilStates();

private:
	int		mScreenWidth;
	int		mScreenHeight;
	float	mScreenNear;
	float	mScreenDepth;

private:
	bool	m_vsync_enabled;
	int		m_videoCardMemory;
	char	m_videoCardDescription[128];

	IDXGISwapChain*				m_swapChain;
	ID3D10Device*				m_device;
	ID3D10RenderTargetView*		m_renderTargetView;
	ID3D10Texture2D*			m_depthStencilBuffer;

	D3D10_VIEWPORT				mViewport;

	ID3D10BlendState*			mTransparentBS;
	ID3D10BlendState*			mDrawReflectionBS;
	ID3D10BlendState*			mAlphaEnableBlendingState;
	ID3D10BlendState*			mAlphaDisableBlendingState;

	ID3D10DepthStencilState*	mDrawMirrorDSS;
	ID3D10DepthStencilState*	mDrawReflectionDSS;
	ID3D10DepthStencilState*	mDepthDisabledStencilState;

	ID3D10RasterizerState*		mCurrentRasterizer;
	ID3D10RasterizerState*		mRasterizerSolid;
	ID3D10RasterizerState*		mDynamicRasterizer;
	ID3D10RasterizerState*		mRasterizerWireframe;
	ID3D10RasterizerState*		mRasterizerCullCWRS;

	ID3D10DepthStencilState*	m_depthStencilState;
	ID3D10DepthStencilView*		m_depthStencilView;

	D3DXMATRIX					m_projectionMatrix;
	D3DXMATRIX					m_worldMatrix;
	D3DXMATRIX					m_orthoMatrix;
};

#endif