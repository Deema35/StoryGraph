// Copyright 2016 Dmitriy Pavlov

#pragma once

#include "Core.h"
#include "CoreUObject.h"
#include "Engine.h"


#include "Engine/EngineTypes.h"
#include "SlateCore.h"
#include "SlateBasics.h"
#include "ModuleManager.h"
#include "StoryGraphPluginRuntimeClasses.h"
#include "EditorStyleSet.h"
#include "EdGraph/EdGraphNodeUtils.h"
#include "ScopedTransaction.h"
#include "EdGraph/EdGraphPin.h"

DECLARE_LOG_CATEGORY_EXTERN(StoryGraphEditor, All, All)

class FStoryGraphEditorModule : public IModuleInterface
{

public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
	
};