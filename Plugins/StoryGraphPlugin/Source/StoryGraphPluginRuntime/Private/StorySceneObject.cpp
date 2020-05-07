// Copyright 2016 Dmitriy Pavlov

#include "StorySceneObject.h"

void IStorySceneObject::RefreshSceneObjectsActive()
{
	bool IsActive = false;

	for (int i = 0; i < OwningStoryGraphObject.Num(); i++)
	{
		if (OwningStoryGraphObject[i]->IsSceneObjectActive)
		{
			IsActive = true;
			break;
		}
	}
	EnableObjectOnMap(IsActive);
}