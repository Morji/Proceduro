#include "BaseGameObject.h"
#include "Transform.h"

BaseGameObject::BaseGameObject(void)
{
	
}


BaseGameObject::~BaseGameObject(void)
{
	std::map<componentType_t, Component*>::iterator itr = mComponentMap.begin();
	while (itr != mComponentMap.end()) {
		if (itr->second != nullptr){
			delete itr->second;
			itr++;
		}
		else{
			itr++;
		}
	}
	mComponentMap.clear();
}

Component *BaseGameObject::GetComponent(componentType_t componentType){
	return mComponentMap[componentType];
}

Transform *BaseGameObject::GetTransform(){
	return (Transform*)mComponentMap[TRANSFORM];
}

void BaseGameObject::AddComponent(Component *component,componentType_t componentType){
	mComponentMap[componentType] = component;
}