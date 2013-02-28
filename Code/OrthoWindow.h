////////////////////////////////////////////////////////////////////////////////
// Filename: OrthoWindow.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _OrthoWindow_H_
#define _OrthoWindow_H_


//////////////
// INCLUDES //
//////////////
#include "Vertex.h"
#include "d3dUtil.h"


////////////////////////////////////////////////////////////////////////////////
// Class name: OrthoWindow
////////////////////////////////////////////////////////////////////////////////
class OrthoWindow
{
public:
	OrthoWindow();
	OrthoWindow(const OrthoWindow&);
	~OrthoWindow();

	bool Initialize(ID3D10Device*, float, float, float, float);
	void Shutdown();
	void Render(ID3D10Device*);

	int GetIndexCount();

private:
	bool InitializeBuffers(ID3D10Device*,float, float, float,float);
	void ShutdownBuffers();
	void RenderBuffers(ID3D10Device*);

private:
	ID3D10Buffer *m_vertexBuffer, *m_indexBuffer;
	int m_vertexCount, m_indexCount;
};

#endif