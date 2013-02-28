////////////////////////////////////////////////////////////////////////////////
// Filename: RenderTexture.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _RENDERTEXTURE_H_
#define _RENDERTEXTURE_H_


//////////////
// INCLUDES //
//////////////
#include <d3d10.h>
#include <d3dx10math.h>


////////////////////////////////////////////////////////////////////////////////
// Class name: RenderTexture
////////////////////////////////////////////////////////////////////////////////
class RenderTexture
{
public:
	RenderTexture();
	RenderTexture(const RenderTexture&);
	~RenderTexture();

	bool Initialize(ID3D10Device*, int, int, float, float);
	void Shutdown();

	void SetRenderTarget(ID3D10Device*);
	void ClearRenderTarget(ID3D10Device*, float, float, float, float);
	ID3D10ShaderResourceView* GetShaderResourceView();

	void GetProjectionMatrix(D3DXMATRIX&);
	void GetOrthoMatrix(D3DXMATRIX&);

	int GetTextureWidth();
	int GetTextureHeight();

private:
	int							m_textureWidth;
	int							m_textureHeight;
	ID3D10Texture2D*			m_renderTargetTexture;
	ID3D10RenderTargetView*		m_renderTargetView;
	ID3D10ShaderResourceView*	m_shaderResourceView;
	ID3D10Texture2D*			m_depthStencilBuffer;
	ID3D10DepthStencilView*		m_depthStencilView;
	D3D10_VIEWPORT				m_viewport;
	D3DXMATRIX					m_projectionMatrix;
	D3DXMATRIX					m_orthoMatrix;
};

#endif