////////////////////////////////////////////////////////////////////////////////
// Filename: OrthoWindow.cpp
////////////////////////////////////////////////////////////////////////////////
#include "OrthoWindow.h"


OrthoWindow::OrthoWindow()
{
	m_vertexBuffer = 0;
	m_indexBuffer = 0;
}


OrthoWindow::OrthoWindow(const OrthoWindow& other)
{
}


OrthoWindow::~OrthoWindow()
{
}


bool OrthoWindow::Initialize(ID3D10Device* device, float xStart, float yStart, float xEnd, float yEnd)
{
	bool result;


	// Initialize the vertex and index buffer that hold the geometry for the ortho window model.
	result = InitializeBuffers(device, xStart, yStart, xEnd, yEnd);
	if(!result)
	{
		return false;
	}

	return true;
}


void OrthoWindow::Shutdown()
{
	// Release the vertex and index buffers.
	ShutdownBuffers();

	return;
}


void OrthoWindow::Render(ID3D10Device* device)
{
	// Put the vertex and index buffers on the graphics pipeline to prepare them for drawing.
	RenderBuffers(device);

	return;
}


int OrthoWindow::GetIndexCount()
{
	return m_indexCount;
}


bool OrthoWindow::InitializeBuffers(ID3D10Device* device, float xStart, float xEnd, float yStart, float yEnd)
{
	float left, right, top, bottom;
	VertexT* vertices;
	unsigned long* indices;
	D3D10_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
    D3D10_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;
	int i;


	// Calculate the screen coordinates of the left side of the window.
	left = xStart;

	// Calculate the screen coordinates of the right side of the window.
	right = xEnd;

	// Calculate the screen coordinates of the top of the window.
	top = -yStart;

	// Calculate the screen coordinates of the bottom of the window.
	bottom = -yEnd;
	
	// Set the number of vertices in the vertex array.
	m_vertexCount = 6;

	// Set the number of indices in the index array.
	m_indexCount = m_vertexCount;

	// Create the vertex array.
	vertices = new VertexT[m_vertexCount];
	if(!vertices)
	{
		return false;
	}

	// Create the index array.
	indices = new unsigned long[m_indexCount];
	if(!indices)
	{
		return false;
	}

	// Load the vertex array with data.
	// First triangle.
	vertices[0].pos = D3DXVECTOR3(left, top, 0.0f);  // Top left.
	vertices[0].texC = D3DXVECTOR2(0.0f, 0.0f);

	vertices[1].pos = D3DXVECTOR3(right, bottom, 0.0f);  // Bottom right.
	vertices[1].texC = D3DXVECTOR2(1.0f, 1.0f);

	vertices[2].pos = D3DXVECTOR3(left, bottom, 0.0f);  // Bottom left.
	vertices[2].texC = D3DXVECTOR2(0.0f, 1.0f);

	// Second triangle.
	vertices[3].pos = D3DXVECTOR3(left, top, 0.0f);  // Top left.
	vertices[3].texC = D3DXVECTOR2(0.0f, 0.0f);

	vertices[4].pos = D3DXVECTOR3(right, top, 0.0f);  // Top right.
	vertices[4].texC = D3DXVECTOR2(1.0f, 0.0f);

	vertices[5].pos = D3DXVECTOR3(right, bottom, 0.0f);  // Bottom right.
	vertices[5].texC = D3DXVECTOR2(1.0f, 1.0f);

	// Load the index array with data.
	for(i=0; i<m_indexCount; i++)
	{
		indices[i] = i;
	}

	// Set up the description of the vertex buffer.
    vertexBufferDesc.Usage = D3D10_USAGE_DEFAULT;
    vertexBufferDesc.ByteWidth = sizeof(VertexT) * m_vertexCount;
    vertexBufferDesc.BindFlags = D3D10_BIND_VERTEX_BUFFER;
    vertexBufferDesc.CPUAccessFlags = 0;
    vertexBufferDesc.MiscFlags = 0;

	// Give the subresource structure a pointer to the vertex data.
    vertexData.pSysMem = vertices;

	// Now finally create the vertex buffer.
    result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer);
	if(FAILED(result))
	{
		return false;
	}

	// Set up the description of the index buffer.
    indexBufferDesc.Usage = D3D10_USAGE_DEFAULT;
    indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_indexCount;
    indexBufferDesc.BindFlags = D3D10_BIND_INDEX_BUFFER;
    indexBufferDesc.CPUAccessFlags = 0;
    indexBufferDesc.MiscFlags = 0;

	// Give the subresource structure a pointer to the index data.
    indexData.pSysMem = indices;

	// Create the index buffer.
	result = device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer);
	if(FAILED(result))
	{
		return false;
	}

	// Release the arrays now that the vertex and index buffers have been created and loaded.
	delete [] vertices;
	vertices = 0;

	delete [] indices;
	indices = 0;

	return true;
}


void OrthoWindow::ShutdownBuffers()
{
	// Release the index buffer.
	if(m_indexBuffer)
	{
		m_indexBuffer->Release();
		m_indexBuffer = 0;
	}

	// Release the vertex buffer.
	if(m_vertexBuffer)
	{
		m_vertexBuffer->Release();
		m_vertexBuffer = 0;
	}

	return;
}


void OrthoWindow::RenderBuffers(ID3D10Device* device)
{
	unsigned int stride;
	unsigned int offset;


	// Set vertex buffer stride and offset.
    stride = sizeof(VertexT); 
	offset = 0;
    
	// Set the vertex buffer to active in the input assembler so it can be rendered.
	device->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);

    // Set the index buffer to active in the input assembler so it can be rendered.
    device->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

    // Set the type of primitive that should be rendered from this vertex buffer, in this case triangles.
    device->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	return;
}