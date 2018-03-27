// Copyright 2016 Dmitriy Pavlov

#include "AssetFactory_StoryGraph.h"
#include "StoryGraph.h"
#include "Kismet2/KismetEditorUtilities.h"


/////////////////////////////////////////////////////
// UAssetFactory_StoryGraph
UAssetFactory_StoryGraph::UAssetFactory_StoryGraph(const FObjectInitializer& ObjectInitializer ): Super(ObjectInitializer)
{
	bCreateNew = true;
	bEditAfterNew = true;
	SupportedClass = UStoryGraphBlueprint::StaticClass();
	ParentClass = AStoryGraphActor::StaticClass();
}

/*UObject* UAssetFactory_StoryGraph::FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
	UStoryGraph* Object = NewObject<UStoryGraph>(InParent, Class, Name, Flags | RF_Transactional);
	return Object;
}*/

UObject* UAssetFactory_StoryGraph::FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{

	return FactoryCreateNew(Class, InParent, Name, Flags, Context, Warn, NAME_None);
}

UObject* UAssetFactory_StoryGraph::FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn, FName CallingContext)
{
	
	UClass* BlueprintClass = UStoryGraphBlueprint::StaticClass();
	UClass* BlueprintGeneratedClass = UBlueprintGeneratedClass::StaticClass();

	UBlueprint* NewBlueprint = FKismetEditorUtilities::CreateBlueprint(ParentClass, InParent, Name, BPTYPE_Normal, BlueprintClass, BlueprintGeneratedClass, CallingContext);
	((UStoryGraphBlueprint*)NewBlueprint)->StoryGraph = NewObject<UStoryGraph>(NewBlueprint);

	return NewBlueprint;
}