#include "BaseGameObject.h"


BaseGameObject::BaseGameObject(void)
{
	mTransform = new Transform();
	AddComponent(mTransform);
}


BaseGameObject::~BaseGameObject(void)
{
	while (mComponentCollection.empty()){
		IComponent *component = mComponentCollection.back();
		if (component){
			delete component;
			component = nullptr;
		}
		mComponentCollection.pop_back();
	}
	mComponentCollection.clear();
}

/*template<class T>
T *BaseGameObject::GetComponent(){
	for (int i = 0; i < mComponentCollection.size(); i++){
		if (typeid(T) == typeid(*mComponentCollection[i])){
			return static_cast<T>(mComponentCollection[i]);
		}
	}
	return 0;
}*/

Transform *BaseGameObject::GetTransform(){
	return mTransform;
}

void BaseGameObject::AddComponent(IComponent *component){
	mComponentCollection.push_back(component);
}