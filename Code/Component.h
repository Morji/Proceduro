/*************************************************************
Component Class: Base class for game object components.

Author: Valentin Hinov
Date: 13/03/2013
Version: 1.0

Exposes: Component
**************************************************************/

#ifndef _H_COMPONENT_H
#define _H_COMPONENT_H

class BaseGameObject;

class Component{
public:
	// optional pointer to parent game object
	Component(BaseGameObject *parent = nullptr);

	~Component();

protected:
	BaseGameObject *pParent;
};

#endif