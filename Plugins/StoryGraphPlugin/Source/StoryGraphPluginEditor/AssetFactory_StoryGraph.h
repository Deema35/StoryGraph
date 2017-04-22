// Copyright 2016 Dmitriy Pavlov
#pragma once
#include "Engine/DataAsset.h"
#include "Factories/Factory.h"
#include "AssetFactory_StoryGraph.generated.h"


UCLASS()
class STORYGRAPHPLUGINEDITOR_API UAssetFactory_StoryGraph : public UFactory
{
	GENERATED_UCLASS_BODY()

	UPROPERTY(EditAnywhere, Category = BlueprintFactory, meta = (AllowAbstract = "", BlueprintBaseOnly = ""))
		TSubclassOf<class UObject> ParentClass;

	// UFactory interface
	virtual UObject* FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn, FName CallingContext) override;
	virtual UObject* FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn) override;
	
	// End of UFactory interface

};