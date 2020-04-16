// Copyright 2016 Dmitriy Pavlov

#pragma once

#include "EngineMinimal.h"
#include "ModuleManager.h"
//#include "StoryGraphPluginRuntimeClasses.h"
//#include "EditorStyleSet.h"
//#include "EdGraph/EdGraphNodeUtils.h"
//#include "ScopedTransaction.h"
//#include "EdGraph/EdGraphPin.h"




class FStoryGraphEditorModule : public IModuleInterface
{

public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
	
};