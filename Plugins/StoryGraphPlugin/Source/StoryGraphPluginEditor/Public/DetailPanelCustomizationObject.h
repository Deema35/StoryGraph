// Copyright 2016 Dmitriy Pavlov
#pragma once

#include "IDetailCustomization.h"
#include "Runtime/SlateCore/Public/Types/SlateEnums.h"

class FStoryGraphObjectDetail : public IDetailCustomization
{
public:
	//FStoryGraphObjectDetail() {}

	/** Makes a new instance of this detail layout class for a specific detail view requesting it */
	static TSharedRef<IDetailCustomization> MakeInstance();

	/** IDetailCustomization interface */
	virtual void CustomizeDetails(IDetailLayoutBuilder& DetailBuilder) override;

	static class UStoryGraphObject* GetDetailObject(IDetailLayoutBuilder* DetailBuilder);

	void CommentCommitted(const FText& NewText, ETextCommit::Type TextType, IDetailLayoutBuilder* DetailBuilder);

protected:
	class IDetailCategoryBuilder* pMainPanel;
};

class FStoryGraphObjectWithSceneObjectDetail : public FStoryGraphObjectDetail
{
public:
	//FStoryGraphObjectWithSceneObjectDetail() {}

	/** Makes a new instance of this detail layout class for a specific detail view requesting it */
	static TSharedRef<IDetailCustomization> MakeInstance();

	/** IDetailCustomization interface */
	virtual void CustomizeDetails(IDetailLayoutBuilder& DetailBuilder) override;
protected:
	class IDetailCategoryBuilder* pScenObjectsPanel;
};

class FStoryGraphCharacterDetail : public FStoryGraphObjectWithSceneObjectDetail
{
public:

	static class FReply EditDialogButtonClick(IDetailLayoutBuilder* DetailBuilder);

	/** Makes a new instance of this detail layout class for a specific detail view requesting it */
	static TSharedRef<IDetailCustomization> MakeInstance();

	/** IDetailCustomization interface */
	virtual void CustomizeDetails(IDetailLayoutBuilder& DetailBuilder) override;

	void DefaultAnswerCommitted(const FText& NewText, ETextCommit::Type TextType, IDetailLayoutBuilder* DetailBuilder);
};

class FStoryGraphPlaceTriggerDetail : public FStoryGraphObjectWithSceneObjectDetail
{
public:

	static class FReply EditMessageButtonClick(IDetailLayoutBuilder* DetailBuilder);

	/** Makes a new instance of this detail layout class for a specific detail view requesting it */
	static TSharedRef<IDetailCustomization> MakeInstance();

	/** IDetailCustomization interface */
	virtual void CustomizeDetails(IDetailLayoutBuilder& DetailBuilder) override;

	void DefaultAnswerCommitted(const FText& NewText, ETextCommit::Type TextType, IDetailLayoutBuilder* DetailBuilder);
};

class FStoryGraphInventoryItemDetail : public FStoryGraphObjectDetail
{
public:

	/** Makes a new instance of this detail layout class for a specific detail view requesting it */
	static TSharedRef<IDetailCustomization> MakeInstance();

	/** IDetailCustomization interface */
	virtual void CustomizeDetails(IDetailLayoutBuilder& DetailBuilder) override;
};

class FStoryGraphOthersDetail : public FStoryGraphObjectWithSceneObjectDetail
{
public:

	/** Makes a new instance of this detail layout class for a specific detail view requesting it */
	static TSharedRef<IDetailCustomization> MakeInstance();

	/** IDetailCustomization interface */
	virtual void CustomizeDetails(IDetailLayoutBuilder& DetailBuilder) override;
};
