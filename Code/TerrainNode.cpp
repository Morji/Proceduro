#include "TerrainNode.h"


TerrainNode::~TerrainNode(void)
{
	for (int i = 0; i < 4; i++){
		if (childNodes[i]){
			delete childNodes[i];
			childNodes[i] = nullptr;
		}
		
	}
}
