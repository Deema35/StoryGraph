// Copyright 2016 Dmitriy Pavlov
#pragma once

#include "IDetailCustomization.h"

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

class FStoryGraphObjectWithScenObjectDetail : public FStoryGraphObjectDetail
{
public:
	//FStoryGraphObjectWithScenObjectDetail() {}

	/** Makes a new instance of this detail layout class for a specific detail view requesting it */
	static TSharedRef<IDetailCustomization> MakeInstance();

	/** IDetailCustomization interface */
	virtual void CustomizeDetails(IDetailLayoutBuilder& DetailBuilder) override;
protected:
	class IDetailCategoryBuilder* pScenObjectsPanel;
};

class FStoryGraphCharecterDetail : public FStoryGraphObjectWithScenObjectDetail
{
public:

	static class FReply EditDialogButtonClick(IDetailLayoutBuilder* DetailBuilder);

	/** Makes a new instance of this detail layout class for a specific detail view requesting it */
	static TSharedRef<IDetailCustomization> MakeInstance();

	/** IDetailCustomization interface */
	virtual void CustomizeDetails(IDetailLayoutBuilder& DetailBuilder) override;

	void DefaultAnswerCommitted(const FText& NewText, ETextCommit::Type TextType, IDetailLayoutBuilder* DetailBuilder);
	
};

class FStoryGraphPlaceTriggerDetail : public FStoryGraphObjectWithScenObjectDetail
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

class FStoryGraphOthersDetail : public FStoryGraphObjectWithScenObjectDetail
{
public:

	/** Makes a new instance of this detail layout class for a specific detail view requesting it */
	static TSharedRef<IDetailCustomization> MakeInstance();

	/** IDetailCustomization interface */
	virtual void CustomizeDetails(IDetailLayoutBuilder& DetailBuilder) override;

};