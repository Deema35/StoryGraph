// Copyright 2016 Dmitriy Pavlov


#include "Commands_StoryGraph.h"
 

void FCommands_StoryGraph::RegisterCommands()
{
#define LOCTEXT_NAMESPACE "Commands_StoryGraphCommands"
	UI_COMMAND(CheckObjects, "Check Objects", "Check Objects", EUserInterfaceActionType::Button, FInputChord());
	UI_COMMAND(SaveAsset, "Save Asset", "Save Asset", EUserInterfaceActionType::Button, FInputChord());
	UI_COMMAND(ExportInXML, "Export in XML", "Export in XML file", EUserInterfaceActionType::Button, FInputChord());
	UI_COMMAND(ImportFromXML, "Import from XML", "Import from XML file", EUserInterfaceActionType::Button, FInputChord());
	UI_COMMAND(FindInContentBrowser, "Find in CB", "Find in Content Browser...", EUserInterfaceActionType::Button, FInputChord());
	UI_COMMAND(UnlinkAllObjects, "Unlink objects", "Unlink all StoryGraph Objects", EUserInterfaceActionType::Button, FInputChord());
#undef LOCTEXT_NAMESPACE
}

