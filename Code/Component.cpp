#include "Component.h"
#include "BaseGameObject.h"

Component::Component(BaseGameObject *parent){
	pParent = parent;
}

Component::~Component(){
	pParent = nullptr;
}