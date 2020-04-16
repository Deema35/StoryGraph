// Copyright 2016 Dmitriy Pavlov
#pragma once
#include "EdGraph/EdGraphSchema.h"
#include "GraphSchema_StoryGraph.generated.h"


class FMenuBuilder;
class UToolMenu;
enum class ENodeType : uint8;

/** Action to add a node to the graph */
USTRUCT()
struct STORYGRAPHPLUGINEDITOR_API FCustomSchemaAction_NewNode : public FEdGraphSchemaAction
{
	GENERATED_USTRUCT_BODY();


	ENodeType NodeType;

	class UStoryGraphObject* OwnedObject;

	class UEdGraphNode* ActorNode;

	FCustomSchemaAction_NewNode()
		: FEdGraphSchemaAction()
	{
	}

	FCustomSchemaAction_NewNode(const FText& InNodeCategory, const FText& InMenuDesc, const FText& InToolTip,
	                            const int32 InGrouping)
		: FEdGraphSchemaAction(InNodeCategory, InMenuDesc, InToolTip, InGrouping)
	{
	}

	// FEdGraphSchemaAction interface
	virtual UEdGraphNode* PerformAction(class UEdGraph* ParentGraph, UEdGraphPin* FromPin, const FVector2D Location,
	                                    bool bSelectNewNode = true) override;
	virtual void AddReferencedObjects(FReferenceCollector& Collector) override;

	// End of FEdGraphSchemaAction interface


	static UEdGraphNode* SpawnNode(ENodeType NodeType, UStoryGraphObject* OwnedObject, class UEdGraph* ParentGraph,
	                               UEdGraphPin* FromPin, const FVector2D Location, bool bSelectNewNode = true);
};

enum class ENodeType : uint8;

UCLASS()
class STORYGRAPHPLUGINEDITOR_API UEdGraphSchema_Base : public UEdGraphSchema
{
	GENERATED_BODY()

public:
	enum class EInsertNodeType SuitableDependedNodesType;
	enum class EInsertNodeType SuitableStandaloneNodesType;

public:
	static void AddAction(class UStoryGraphObject* OwnedObject, const ENodeType NodeType, FString Category,
	                      TArray<TSharedPtr<FEdGraphSchemaAction>>& OutActions, UEdGraph* OwnerOfTemporaries,
	                      int InGrouping);


protected:
	// Begin EdGraphSchema interface
	virtual void GetGraphContextActions(FGraphContextMenuBuilder& ContextMenuBuilder) const override;
	//virtual void GetContextMenuActions(const UEdGraph* CurrentGraph, const UEdGraphNode* InGraphNode,
	//                                   const UEdGraphPin* InGraphPin, FMenuBuilder* MenuBuilder,
	//                                   bool bIsDebugging) const override;
	virtual void GetContextMenuActions(UToolMenu* Menu, UGraphNodeContextMenuContext* Context) const override;
	virtual const FPinConnectionResponse CanCreateConnection(const UEdGraphPin* A, const UEdGraphPin* B) const override;
	virtual FLinearColor GetPinTypeColor(const FEdGraphPinType& PinType) const override;
	virtual bool ShouldHidePinDefaultValue(UEdGraphPin* Pin) const override;
	// End EdGraphSchema interface
};


UCLASS()
class STORYGRAPHPLUGINEDITOR_API UEdGraphSchema_StoryGraph : public UEdGraphSchema_Base
{
	GENERATED_BODY()

public:
	UEdGraphSchema_StoryGraph();

protected:
	// Begin EdGraphSchema interface
	virtual const FPinConnectionResponse CanCreateConnection(const UEdGraphPin* A, const UEdGraphPin* B) const override;
	virtual class FConnectionDrawingPolicy* CreateConnectionDrawingPolicy(
		int32 InBackLayerID, int32 InFrontLayerID, float InZoomFactor, const FSlateRect& InClippingRect,
		class FSlateWindowElementList& InDrawElements, class UEdGraph* InGraphObj) const override;
	// End EdGraphSchema interface
};

UCLASS()
class STORYGRAPHPLUGINEDITOR_API UEdGraphSchema_DialogGraph : public UEdGraphSchema_Base
{
	GENERATED_BODY()
public:
	UEdGraphSchema_DialogGraph();

protected:
	virtual const FPinConnectionResponse CanCreateConnection(const UEdGraphPin* A, const UEdGraphPin* B) const override;
	virtual class FConnectionDrawingPolicy* CreateConnectionDrawingPolicy(
		int32 InBackLayerID, int32 InFrontLayerID, float InZoomFactor, const FSlateRect& InClippingRect,
		class FSlateWindowElementList& InDrawElements, class UEdGraph* InGraphObj) const override;
};


UCLASS()
class STORYGRAPHPLUGINEDITOR_API UEdGraphSchema_MessageGraph : public UEdGraphSchema_DialogGraph
{
	GENERATED_BODY()

public:
	UEdGraphSchema_MessageGraph();
};
