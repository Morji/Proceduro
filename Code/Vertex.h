#ifndef VERTEX_H
#define VERTEX_H

#include "d3dUtil.h"

typedef D3DXVECTOR2 Vector2f;
typedef D3DXVECTOR3 Vector3f;
typedef D3DXVECTOR4 Vector4f;
typedef D3DXCOLOR	Color;

struct VertexNT
{
	Vector3f	pos;
	Vector3f	normal;
	Vector2f	texC;
};

struct VertexC
{
	Vector3f	pos;
	Color		color;
};

struct VertexT
{
	Vector3f	pos;
	Vector2f	texC;
};

struct VertexNC
{
	Vector3f	pos;
	Vector3f	normal;
	Color		color;
};

struct Vertex
{
	Vector3f	pos;
};

 
#endif // VERTEX_H