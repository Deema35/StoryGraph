// Copyright 2016 Dmitriy Pavlov
#pragma once

#include "StoryGraphObject.h"

class STORYGRAPHPLUGINRUNTIME_API IStorySceneObject
{
public:
	virtual ~IStorySceneObject()
	{
	}

	TArray<UStoryGraphObjectWithSceneObject*> OwningStoryGraphObject;

	bool IsEnabled;


public:
	IStorySceneObject() : IsEnabled(true)
	{
	}

	virtual void RefreshSceneObjectsActive();
	virtual void EnableObjectOnMap(bool IsEnabled) = 0;
	virtual void SetStoryGraphObjectState(int NewState) = 0;
	virtual void SendMessageToSceneObject(FString Message) = 0;
};




