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

#include "Component.h"
#include "d3dUtil.h"
#include <map>

// allows fast access of the transform component
class Transform;

enum componentType_t{TRANSFORM,
					 BOUNDING_BOX,
					 RENDERER};

class BaseGameObject{
public:
	BaseGameObject(void);
	~BaseGameObject(void);

	virtual bool Initialize(ID3D10Device* device, HWND hwnd) = 0;

	Component *GetComponent(componentType_t componentType);
	
	Transform *GetTransform();

private:
	std::map<componentType_t,Component*> mComponentMap;
	
protected:
	void AddComponent(Component *component,componentType_t componentType);
};

#endif