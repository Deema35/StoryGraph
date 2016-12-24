// Copyright 2016 Dmitriy Pavlov
#pragma once
#include "Developer/AssetTools/Public/AssetToolsModule.h"
#include "Developer/AssetTools/Public/AssetTypeActions_Base.h"

class STORYGRAPHPLUGINEDITOR_API FAssetTypeActions_StoryGraph : public FAssetTypeActions_Base
{
public:

	virtual FText GetName() const override { return FText::FromString("StoryGraph"); }

	virtual FColor GetTypeColor() const override { return FColor::Cyan; }

	virtual FText GetAssetDescription(const FAssetData & AssetData) const override;

	virtual UClass* GetSupportedClass() const override; 

	virtual bool HasActions(const TArray<UObject*>& InObjects) const override { return true; }

	virtual void GetActions(const TArray<UObject*>& InObjects, FMenuBuilder& MenuBuilder);

	virtual bool CanLocalize() const override { return true; }

	virtual void OpenAssetEditor(const TArray<UObject*>& InObjects, TSharedPtr<class IToolkitHost> EditWithinLevelEditor = TSharedPtr<IToolkitHost>()) override;

	virtual uint32 GetCategories() override { return EAssetTypeCategories::Misc | EAssetTypeCategories::Basic; }


	static void RegistrateCustomPartAssetType();

private:

	void AddToScen(class UStoryGraphBlueprint* PropData);

	void RemoveFromScen(UStoryGraphBlueprint* PropData);

};