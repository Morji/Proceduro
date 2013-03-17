#include "Transform.h"

void Transform::GetTransformMatrix(D3DXMATRIX &matrix){
	D3DXMATRIX m;
	D3DXMatrixIdentity(&matrix);
	D3DXMatrixScaling(&m, scale.x, scale.y, scale.z);
	matrix*=m;
	D3DXMatrixRotationX(&m, rotation.x);
	matrix*=m;
	D3DXMatrixRotationY(&m, rotation.y);
	matrix*=m;
	D3DXMatrixRotationZ(&m, rotation.z);	
	matrix*=m;
	D3DXMatrixTranslation(&m, position.x, position.y, position.z);
	matrix*=m;
}