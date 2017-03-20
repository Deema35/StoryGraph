// Copyright 2016 Dmitriy Pavlov
#pragma once

#include "EdGraph/EdGraphNode.h"
#include "StoryGraphObject.h"
#include "CustomNods.h"
#include "ProxyNods.generated.h"

class UStoryGraphObject;
enum class ENodeType : uint8;

UCLASS()

class STORYGRAPHPLUGINEDITOR_API UProxyNodeBase : public UEdGraphNode 
{
	GENERATED_BODY()

		
public:
	UPROPERTY()
	bool bUserDefined;

	UPROPERTY()
		UCustomNodeBase* CustomNode;

public:

	UProxyNodeBase() : bUserDefined(true){}


	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;

	virtual FLinearColor GetNodeTitleColor() const override;


	bool CycleInspection(uint32 UniqueID);

	virtual void DoubleClicke() {}

	virtual FText GetTooltipText() const override;

	/** Whether or not this node can be safely duplicated (via copy/paste, etc...) in the graph */
	virtual bool CanDuplicateNode() const override;

	/** Whether or not this node can be deleted by user action */
	virtual bool CanUserDeleteNode() const override;

	virtual void PinConnectionListChanged(UEdGraphPin * Pin) override;

	virtual void PostLoad() override;

	virtual void AllocateDefaultPins() override;

	void HandleCreatePin(FStoryGraphPin NewPin);

	void HandleRemovePin(int32 PinNumber);

	void HandleBreakPin(int32 PinNumber);

	virtual void DestroyNode() override;
};


