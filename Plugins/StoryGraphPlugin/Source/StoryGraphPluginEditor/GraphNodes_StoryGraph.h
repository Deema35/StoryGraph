// Copyright 2016 Dmitriy Pavlov
#pragma once

#include "SGraphNode.h"
#include "AssetThumbnail.h"
#include "CustomNods.h"
#include "SGraphPin.h"





//PineNode....................................................................................
class  SCustomOutputPin : public SGraphPin
{
public:
	SLATE_BEGIN_ARGS(SCustomOutputPin) {}
	SLATE_END_ARGS()

		void Construct(const FArguments& InArgs, UEdGraphPin* InPin, int PinNumber = 0);
protected:
	// Begin SGraphPin interface
	virtual TSharedRef<SWidget>	GetDefaultValueWidget() override;
	// End SGraphPin interface

	/** @return The color that we should use to draw this pin */
	virtual FSlateColor GetPinColor() const override;

	// End SGraphPin interface

	const FSlateBrush* GetPinBorder() const;

	FMargin GetPadding() const;

private:

	bool IsPinNumbered;
};

//CustomNodeBase.............................................................................................................
class  SGraphNode_CustomNodeBase : public SGraphNode
{
public:
	SLATE_BEGIN_ARGS(SGraphNode_CustomNodeBase) { }
	SLATE_END_ARGS()

	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs, UProxyNodeBase* InNode);

	// SGraphNode interface
	virtual void UpdateGraphNode() override;
	virtual void CreatePinWidgets() override;
	virtual void AddPin(const TSharedRef<SGraphPin>& PinToAdd) override;
	virtual void CreateNodeWidget();
	// End of SGraphNode interface

protected:
	class UProxyNodeBase* NodeBace;
	virtual const FSlateBrush* GetNameIcon() const;
	TSharedPtr<SHorizontalBox> TopNodeBox;
	TSharedPtr<SHorizontalBox> BottomNodeBox;
	TSharedPtr<SOverlay> NodeWiget;
	virtual void AddNodeStrings(TSharedPtr<SVerticalBox> NodeBox);
private:
	TSharedPtr<IToolTip> CreateToolTipWiget();
	
};



//StoryGraphDependetNode....................................................................................................................

class SGraphNode_StoryGraphDependetNode : public SGraphNode_CustomNodeBase
{
public:

	virtual void AddNodeStrings(TSharedPtr<SVerticalBox> NodeBox) override;

};



//DialogNodeBase..................................................................................................................
class SGraphNode_DialogBase : public SGraphNode_CustomNodeBase
{
public:

	virtual void UpdateGraphNode() override;
	virtual void CreatePinWidgets() override;
	

protected:
	virtual void AddNodeStrings(TSharedPtr<SVerticalBox> NodeBox) override;
};

//StoryGraphDependetNode....................................................................................................................

class SGraphNode_DialogDependetNode : public SGraphNode_DialogBase
{
public:

	virtual void AddNodeStrings(TSharedPtr<SVerticalBox> NodeBox) override;

};

//DialogRootNode..................................................................................................................
class SGraphNode_DialogRoot : public SGraphNode_DialogBase
{
protected:
	virtual void AddNodeStrings(TSharedPtr<SVerticalBox> NodeBox) override;

};

//DialogNode..................................................................................................................
class SGraphNode_Dialog : public SGraphNode_DialogRoot
{
public:
	virtual void CreatePinWidgets() override;
protected:
	virtual void AddNodeStrings(TSharedPtr<SVerticalBox> NodeBox) override;

};

