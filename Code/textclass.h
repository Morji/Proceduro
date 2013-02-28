////////////////////////////////////////////////////////////////////////////////
// Filename: textclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _TEXTCLASS_H_
#define _TEXTCLASS_H_


///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "fontclass.h"
#include "FontShader.h"


////////////////////////////////////////////////////////////////////////////////
// Class name: TextClass
////////////////////////////////////////////////////////////////////////////////
class TextClass
{
private:
	struct SentenceType
	{
		ID3D10Buffer *vertexBuffer, *indexBuffer;
		int vertexCount, indexCount, maxLength;
		float red, green, blue;
	};

	struct VertexType
	{
		D3DXVECTOR3 position;
	    D3DXVECTOR2 texture;
	};

public:
	TextClass();
	TextClass(const TextClass&);
	~TextClass();

	bool Initialize(ID3D10Device*, HWND, int, int, D3DXMATRIX);
	void Shutdown();
	void Render(ID3D10Device*, D3DXMATRIX, D3DXMATRIX);

	bool SetVideoCardInfo(char*, int);
	bool SetFps(int);
	bool SetCpu(int);
	bool SetCameraPosition(float, float, float);
	bool SetCameraRotation(float, float, float);

private:
	bool InitializeSentence(SentenceType**, int, ID3D10Device* device);
	bool UpdateSentence(SentenceType*, char*, int, int, float, float, float);
	void ReleaseSentence(SentenceType**);
	void RenderSentence(ID3D10Device*, SentenceType*, D3DXMATRIX, D3DXMATRIX);

private:
	FontClass* m_Font;
	FontShader* m_FontShader;
	int m_screenWidth, m_screenHeight;
	D3DXMATRIX m_baseViewMatrix;
	SentenceType *m_sentence1, *m_sentence2, *m_sentence3, *m_sentence4, *m_sentence5;
	SentenceType *m_sentence6, *m_sentence7, *m_sentence8, *m_sentence9, *m_sentence10;
};

#endif