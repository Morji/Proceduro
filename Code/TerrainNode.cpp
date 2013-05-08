#include "TerrainNode.h"
#include "Vertex.h"
#include "GeometryBuilder.h"

TerrainNode::~TerrainNode(void)
{
	for (int i = 0; i < 4; i++){
		if (childNodes[i]){
			delete childNodes[i];
			childNodes[i] = nullptr;
		}
	}
	if (mBoundsRenderer){
		delete mBoundsRenderer;
		mBoundsRenderer = nullptr;
	}
	if (nodeRenderer){
		delete nodeRenderer;
		nodeRenderer = nullptr;
	}
	if (mColorShader){
		delete mColorShader;
		mColorShader = nullptr;
	}
}

void TerrainNode::DrawBounds(ID3DObject *d3dObject, D3DXMATRIX viewMatrix, float maxHeight){
	if (!mBoundsRenderer){
		mBoundsRenderer = new Renderer();
		BuildColoredCube(d3dObject->GetDevice(),mBoundsRenderer, WHITE);
		if (!mColorShader){
			mColorShader = new ColorShader();
			if (!mColorShader->Initialize(d3dObject->GetDevice(),nullptr)){
				return;
			}
		}
	}
	D3DXMATRIX objMatrix,m;
	D3DXMatrixIdentity(&objMatrix);
	D3DXMatrixScaling(&m, diameter/2, maxHeight/2 , diameter/2);
	objMatrix*=m;
	D3DXMatrixTranslation(&m, positionX, maxHeight/2, positionZ);
	objMatrix*=m;
	objMatrix *= *d3dObject->GetWorldMatrix();
	mBoundsRenderer->RenderBuffers(d3dObject->GetDevice());

	//enable wireframe
	d3dObject->TurnWireframeOn();
		mColorShader->Render(d3dObject->GetDevice(),mBoundsRenderer->GetIndexCount(),objMatrix,viewMatrix,*d3dObject->GetProjectionMatrix());
	d3dObject->TurnWireframeOff();
}
