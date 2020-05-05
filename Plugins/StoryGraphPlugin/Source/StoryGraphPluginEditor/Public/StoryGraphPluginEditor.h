// Copyright 2016 Dmitriy Pavlov

#pragma once

#include "EngineMinimal.h"

class FStoryGraphEditorModule : public IModuleInterface
{

public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
	
};