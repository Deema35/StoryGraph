// Copyright 2016 Dmitriy Pavlov

#pragma once

#include "Engine.h"
#include "ModuleManager.h"
#include "StoryGraphPluginRuntimeClasses.h"
#include "EditorStyleSet.h"
#include "EdGraph/EdGraphNodeUtils.h"
#include "ScopedTransaction.h"
#include "EdGraph/EdGraphPin.h"
#include <map>

DECLARE_LOG_CATEGORY_EXTERN(StoryGraphEditor, All, All)

class FStoryGraphEditorModule : public IModuleInterface
{

public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
	
};