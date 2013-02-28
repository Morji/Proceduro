#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

//#ifdef _MSVC
#pragma warning(disable:4244)	// double -> float conversion
#define _CRT_SECURE_NO_WARNINGS
//#endif //_MSVC
#include "d3dUtil.h"
#include "GameTimer.h"
#include "Vertex.h"
#include "TextureLoader.h"
#include <string>

#pragma warning(disable:4305) // double -> float

/*
Base Game Object Class: This handles basic setup and drawing
*/
class GameObject
{

public:
	D3DXMATRIX objMatrix;
	D3DXVECTOR3 pos, rot, scale;	

public:
	GameObject(): mVertexCount(0), mIndexCount(0), md3dDevice(0), mVB(0), mIB(0), scale(1,1,1),pos(0,0,0),rot(0,0,0), mTopologyChoice(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST)
	{
		referenceCount = 0;
		diffuseMap = specularMap = blendMap = 0;
		D3DXMatrixIdentity(&mTexMatrix);
		for (int i = 0; i < 3; i++) diffuseMapRV[i] = 0;
	}

	virtual ~GameObject(){		
		Shutdown();		
	}	

	virtual bool Initialize(ID3D10Device* device);

	bool InitializeWithTexture(ID3D10Device* device, WCHAR* diffuseMapTex, WCHAR* specularMapTex);

	bool InitializeWithMultiTexture(ID3D10Device* device, WCHAR* specularMapTex, WCHAR* blendMapTex, WCHAR* diffuseMapRV1Tex,
																									 WCHAR* diffuseMapRV2Tex,
																									 WCHAR* diffuseMapRV3Tex);
	void AddReference();
	int	 referenceCount;
	void Shutdown();
	void Render(D3DXMATRIX worldMatrix);

	ID3D10ShaderResourceView* GetDiffuseTexture();
	ID3D10ShaderResourceView* GetSpecularTexture();
	ID3D10ShaderResourceView* GetBlendTexture();
	ID3D10ShaderResourceView* GetDiffuseMap(int rvWhich);	

	int	GetIndexCount();
	D3DXMATRIX	&GetTexMatrix();
	///////////////////////////////////////////////
private:
	
	void ShutdownBuffers();
	void RenderBuffers();
	virtual void setTrans(D3DXMATRIX worldMatrix);	
	

	TextureLoader* specularMap;
	TextureLoader* diffuseMap;	
	TextureLoader* blendMap;
	TextureLoader* diffuseMapRV[3];
	///////////////////////////////////////////////
protected:
	D3D_PRIMITIVE_TOPOLOGY	mTopologyChoice;

	DWORD mVertexCount;
	DWORD mIndexCount;
	D3DXMATRIX	   mTexMatrix;

	ID3D10Device* md3dDevice;
	ID3D10Buffer* mVB;
	ID3D10Buffer* mIB;

	unsigned int stride;
	unsigned int offset;

	virtual bool InitializeBuffers(DWORD* indices,  VertexNT* vertices);
	virtual bool SetupArraysAndInitBuffers();

	bool LoadTexture(WCHAR* diffuseMapTex, WCHAR* specularMapTex);
	bool LoadMultiTexture(WCHAR* specularMapTex, WCHAR* blendMapTex, WCHAR* diffuseMapRV1Tex,
																						   WCHAR* diffuseMapRV2Tex,
																						   WCHAR* diffuseMapRV3Tex);
	void ReleaseTexture();
};

#endif // GAMEOBJECT_H

