/*************************************************************
BaseGameObject Class: Provides both and interface
and a base class for any game objects in
the application. Provides basic methods for each game object
class. Any object that extends this must add any components
it uses into the IComponent collection. This class handles
the destruction of components

Author: Valentin Hinov
Date: 14/03/2013
Version: 1.0

Exposes: BaseGameObject
Supported methods: 
Initialize
GetComponent
**************************************************************/

#ifndef _H_BASEGAMEOBJECT_H
#define _H_BASEGAMEOBJECT_H

#include "IComponent.h"
#include "d3dUtil.h"
#include "Transform.h"
#include "BoundingBox.h"
#include <vector>
#include <typeinfo>


class BaseGameObject{
public:
	BaseGameObject(void);
	~BaseGameObject(void);

	virtual bool Initialize(ID3D10Device* device, HWND hwnd) = 0;

	Transform	*GetTransform();

private:
	std::vector<IComponent*>	mComponentCollection;	
	
protected:
	void AddComponent(IComponent *component);	
	Transform *mTransform;
};

#endif