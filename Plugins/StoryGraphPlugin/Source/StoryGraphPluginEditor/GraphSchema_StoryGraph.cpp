// Copyright 2016 Dmitriy Pavlov

#include "GraphSchema_StoryGraph.h"
#include "ScopedTransaction.h"
#include "CustomNods.h"
#include "ProxyNods.h"
#include "EdGraph/EdGraph.h"
#include "EdGraph/EdGraphPin.h"
#include "GenericCommands.h"
#include "GraphEditorActions.h"
#include "ConnectionDrawingPolicy_StoryGraph.h"
#include "StoryGraphObject.h"
#include "StoryGraph.h"
#include "Graph_StoryGraph.h"
#include "AssetEditor_StoryGraph.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"




UEdGraphNode* FCustomSchemaAction_NewNode::PerformAction(UEdGraph* ParentGraph, UEdGraphPin* FromPin, const FVector2D Location, bool bSelectNewNode/* = true*/)
{
	ActorNode = SpawnNode(NodeType, OwnedObject, ParentGraph, FromPin, Location);

	return ActorNode;
}

void FCustomSchemaAction_NewNode::AddReferencedObjects(FReferenceCollector& Collector)
{
	FEdGraphSchemaAction::AddReferencedObjects(Collector);

	// These don't get saved to disk, but we want to make sure the objects don't get GC'd while the action array is around
	Collector.AddReferencedObject(ActorNode);
}

UEdGraphNode* FCustomSchemaAction_NewNode::SpawnNode(ENodeType NodeType, UStoryGraphObject* OwnedObject, UEdGraph* ParentGraph, UEdGraphPin* FromPin, const FVector2D Location, bool bSelectNewNode)
{
	UStoryGraph* pStoryGraph = ((UEdGraph_StoryGraph*)ParentGraph)->GetStoryGraph();
	UObject* CustomNodeParent = ((UEdGraph_StoryGraph*)ParentGraph)->GraphOwner;
	if (NodeType == ENodeType::QuestStart)
	{
		TArray<UProxyNodeBase*> FindNods;
		ParentGraph->GetNodesOfClass<UProxyNodeBase>(FindNods);
		for (int i = 0; i < FindNods.Num(); i++)
		{
			if (FindNods[i]->CustomNode->pGraphObject == OwnedObject &&FindNods[i]->CustomNode->NodeType == ENodeType::QuestStart)
			{
				pStoryGraph->pAssetEditor->FocusWindow();
				pStoryGraph->pAssetEditor->JumpToNode(FindNods[i]);
				return NULL;
			}
		}
	}
	
	UCustomNodeBase* ActorNode = NewObject<UCustomNodeBase>(CustomNodeParent, UCustomNodeBase::GetClassFromNodeType(NodeType));
	ActorNode->SetFlags(RF_Transactional);
	ActorNode->pStoryGraph = ((UEdGraph_StoryGraph*)ParentGraph)->GetStoryGraph();
	
	if (UStoryGraph* StoryGraph = Cast<UStoryGraph>(CustomNodeParent))
	{
		StoryGraph->GarphNods.Add(ActorNode);
	}
	else if (UStoryGraphCharecter* Charecter = Cast<UStoryGraphCharecter>(CustomNodeParent))
	{
		Charecter->GarphNods.Add(ActorNode);
	}
	else if (UStoryGraphPlaceTrigger* PlaceTrigger = Cast<UStoryGraphPlaceTrigger>(CustomNodeParent))
	{
		PlaceTrigger->GarphNods.Add(ActorNode);
	}

	UProxyNodeBase* ProxyNode = NewObject<UProxyNodeBase>((UObject *)ParentGraph, UProxyNodeBase::StaticClass());
	ProxyNode->CustomNode = ActorNode;
	ActorNode->InitNode(OwnedObject);
	ProxyNode->SetFlags(RF_Transactional);
	ParentGraph->AddNode(ProxyNode, true, bSelectNewNode);
	

	ProxyNode->CreateNewGuid();
	ProxyNode->PostPlacedNewNode();
	ProxyNode->AllocateDefaultPins();
	ProxyNode->AutowireNewNode(FromPin);
	ProxyNode->NodePosX = Location.X;
	ProxyNode->NodePosY = Location.Y;
	ProxyNode->SnapToGrid(16);
	

	if (FromPin)
	{
		
		for (int i = 0; i < ProxyNode->Pins.Num(); i++)
		{
			if (FromPin->PinType.PinCategory == ProxyNode->Pins[i]->PinType.PinCategory)
			{
				if (FromPin->Direction != ProxyNode->Pins[i]->Direction)
				{
					ParentGraph->GetSchema()->TryCreateConnection(FromPin, ProxyNode->Pins[i]);
				}
			}
		}
	}
	
	if (ProxyNode->CustomNode->NodeType == ENodeType::DialogNode || ProxyNode->CustomNode->NodeType == ENodeType::Message)
	{
		ProxyNode->CustomNode->CreatePinDelegate.BindUObject(ProxyNode, &UProxyNodeBase::HandleCreatePin);
		ProxyNode->CustomNode->RemovePinDelegate.BindUObject(ProxyNode, &UProxyNodeBase::HandleRemovePin);
		ProxyNode->CustomNode->BreakPinDelegate.BindUObject(ProxyNode, &UProxyNodeBase::HandleBreakPin);
	}

	((UEdGraph_StoryGraph*)ParentGraph)->GetStoryGraph()->StoryGraphState = EStoryGraphState::ST_Modify;

	return ProxyNode;
}
//UEdGraphSchema_Base...........................................................................................



void UEdGraphSchema_Base::GetGraphContextActions(FGraphContextMenuBuilder& ContextMenuBuilder) const
{

	const UEdGraphPin* FromPin = ContextMenuBuilder.FromPin;
	const UEdGraph_StoryGraph* Graph = (UEdGraph_StoryGraph*)ContextMenuBuilder.CurrentGraph;
	const UStoryGraph* StoryGraph = Graph->GetStoryGraph();
	TArray<TSharedPtr<FEdGraphSchemaAction> > Actions;
	
	
	FString NodeCategory;

	
	for (int i = 0; i < StoryGraph->GarphObjects.Num(); i++)
	{
		
		for (int j = 0; j < StoryGraph->GarphObjects[i]->DependetNodes.Num(); j++)
		{
			if (UCustomNodeBase::GetIncertNodeType(StoryGraph->GarphObjects[i]->DependetNodes[j]) == SuitableDependetNodesType)
			{
				
				NodeCategory = UStoryGraphObject::GetObjectTypeEnumAsString(StoryGraph->GarphObjects[i]->ObjectType) + "|" + StoryGraph->GarphObjects[i]->ObjName.ToString();
				AddAction(StoryGraph->GarphObjects[i], StoryGraph->GarphObjects[i]->DependetNodes[j], NodeCategory, Actions, ContextMenuBuilder.OwnerOfTemporaries, 0);
			}
		}
	}

	int i = 0;

	while (i < GetNumberEnums("ENodeType"))
	{
		if (UCustomNodeBase::GetIncertNodeType((ENodeType)i) == SuitableStandaloneNodesType)
		{
			NodeCategory = "Nodes";
			AddAction(NULL, (ENodeType)i, NodeCategory, Actions, ContextMenuBuilder.OwnerOfTemporaries, 1);
			
		}
		i++;
	}

	
	for (TSharedPtr<FEdGraphSchemaAction> Action : Actions)
	{
		ContextMenuBuilder.AddAction(Action);
	}
}



const FPinConnectionResponse UEdGraphSchema_Base::CanCreateConnection(const UEdGraphPin* A, const UEdGraphPin* B) const
{
	UProxyNodeBase* ABase = Cast<UProxyNodeBase>(A->GetOwningNode());
	UProxyNodeBase* BBase = Cast<UProxyNodeBase>(B->GetOwningNode());

	// Make sure the input is connecting to an output
	if (A->Direction == B->Direction)
	{
		return FPinConnectionResponse(CONNECT_RESPONSE_DISALLOW, TEXT("Not allowed"));
	}

	// Make sure the data types match
	if (A->PinType.PinCategory != B->PinType.PinCategory)
	{
		return FPinConnectionResponse(CONNECT_RESPONSE_DISALLOW, TEXT("Not match types"));
	}

	//Make sure not same node
	if (ABase->GetUniqueID() == BBase->GetUniqueID())
	{
		return FPinConnectionResponse(CONNECT_RESPONSE_DISALLOW, TEXT("Same node"));
	}

	//Make sure not loop
	if (!BBase->CycleInspection(ABase->GetUniqueID()))
	{
		return FPinConnectionResponse(CONNECT_RESPONSE_DISALLOW, TEXT("Create loop"));
	}

	return FPinConnectionResponse(CONNECT_RESPONSE_MAKE, TEXT(""));
}

FLinearColor UEdGraphSchema_Base::GetPinTypeColor(const FEdGraphPinType& PinType) const
{
	return FColor::Yellow;
}

bool UEdGraphSchema_Base::ShouldHidePinDefaultValue(UEdGraphPin* Pin) const
{
	return true;
}

void UEdGraphSchema_Base::GetContextMenuActions(const UEdGraph* CurrentGraph, const UEdGraphNode* InGraphNode, const UEdGraphPin* InGraphPin, FMenuBuilder* MenuBuilder, bool bIsDebugging) const
{
	
	MenuBuilder->AddMenuEntry(FGenericCommands::Get().Delete);
	MenuBuilder->AddMenuEntry(FGenericCommands::Get().Cut);
	MenuBuilder->AddMenuEntry(FGenericCommands::Get().Copy);
	MenuBuilder->AddMenuEntry(FGenericCommands::Get().Paste);
	MenuBuilder->AddMenuEntry(FGenericCommands::Get().SelectAll);
	MenuBuilder->AddMenuEntry(FGenericCommands::Get().Duplicate);
	if (InGraphPin)
	{
		MenuBuilder->AddMenuEntry(FGraphEditorCommands::Get().BreakPinLinks);
	}
	else
	{
		MenuBuilder->AddMenuEntry(FGraphEditorCommands::Get().BreakNodeLinks);
	}

	Super::GetContextMenuActions(CurrentGraph, InGraphNode, InGraphPin, MenuBuilder, bIsDebugging);
}


void UEdGraphSchema_Base::AddAction(UStoryGraphObject* OwnedObject, ENodeType NodeType, FString Category, TArray<TSharedPtr<FEdGraphSchemaAction> >& OutActions, UEdGraph* OwnerOfTemporaries, int InGruping)
{
	
	
	FText MenuDesc = FText::FromString(UCustomNodeBase::GetActionNameFromNodeType(NodeType));
	FText ToolTip = FText::FromString(UCustomNodeBase::GetToolTipFromNodeType(NodeType));
	TSharedPtr<FCustomSchemaAction_NewNode> NewActorNodeAction = TSharedPtr<FCustomSchemaAction_NewNode>(new FCustomSchemaAction_NewNode(FText::FromString(Category), MenuDesc, ToolTip, InGruping));
	
	NewActorNodeAction->NodeType = NodeType;
	NewActorNodeAction->OwnedObject = OwnedObject;
	OutActions.Add(NewActorNodeAction);
}
//UEdGraphSchema_DialogGraph..............................................................................................................
UEdGraphSchema_StoryGraph::UEdGraphSchema_StoryGraph()
{
	SuitableDependetNodesType = EIncertNodeType::StoryGraphDependent;
	SuitableStandaloneNodesType = EIncertNodeType::StoryGraphStandalone;
}
const FPinConnectionResponse UEdGraphSchema_StoryGraph::CanCreateConnection(const UEdGraphPin* A, const UEdGraphPin* B) const
{
	FPinConnectionResponse PinConnectionResponse = Super::CanCreateConnection(A, B);

	if (PinConnectionResponse.Response == CONNECT_RESPONSE_DISALLOW)
	{
		return PinConnectionResponse;
	}

	UProxyNodeBase* ABase = Cast<UProxyNodeBase>(A->GetOwningNode());
	UProxyNodeBase* BBase = Cast<UProxyNodeBase>(B->GetOwningNode());

	if (A->PinType.PinCategory.ToString() == UCustomNodeBase::GetPinDataTypeEnumAsString(EPinDataTypes::PinType_Horizontal) && (A->LinkedTo.Num() > 0 || B->LinkedTo.Num() > 0))
	{
		return FPinConnectionResponse(CONNECT_RESPONSE_DISALLOW, TEXT("Horizontal pins have not more the one connection"));
	}

	if (ABase->CustomNode->NodeType == ENodeType::AddQuestPhase && BBase->CustomNode->NodeType == ENodeType::AddQuestPhase)
	{
		return FPinConnectionResponse(CONNECT_RESPONSE_DISALLOW, TEXT("Connect two quest phase pointlessly"));
	}

	return FPinConnectionResponse(CONNECT_RESPONSE_MAKE, TEXT(""));
}

FConnectionDrawingPolicy* UEdGraphSchema_StoryGraph::CreateConnectionDrawingPolicy(int32 InBackLayerID, int32 InFrontLayerID, float InZoomFactor, const FSlateRect& InClippingRect, FSlateWindowElementList& InDrawElements, class UEdGraph* InGraphObj) const
{

	return new FConnectionDrawingPolicy_StoryGraph(InBackLayerID, InFrontLayerID, InZoomFactor, InClippingRect, InDrawElements);
}
//UEdGraphSchema_DialogGraph..............................................................................................................
UEdGraphSchema_DialogGraph::UEdGraphSchema_DialogGraph()
{
	SuitableDependetNodesType = EIncertNodeType::DialogGraphDependent;
	SuitableStandaloneNodesType = EIncertNodeType::DialogGraphStandalone;
}

FConnectionDrawingPolicy* UEdGraphSchema_DialogGraph::CreateConnectionDrawingPolicy(int32 InBackLayerID, int32 InFrontLayerID, float InZoomFactor, const FSlateRect& InClippingRect, class FSlateWindowElementList& InDrawElements, class UEdGraph* InGraphObj) const
{
	return new FConnectionDrawingPolicy(InBackLayerID, InFrontLayerID, InZoomFactor, InClippingRect, InDrawElements);
}

const FPinConnectionResponse UEdGraphSchema_DialogGraph::CanCreateConnection(const UEdGraphPin* A, const UEdGraphPin* B) const
{

	FPinConnectionResponse PinConnectionResponse = Super::CanCreateConnection(A, B);

	if (PinConnectionResponse.Response == CONNECT_RESPONSE_DISALLOW)
	{
		return PinConnectionResponse;
	}
	UProxyNodeBase* ABase = Cast<UProxyNodeBase>(A->GetOwningNode());
	UProxyNodeBase* BBase = Cast<UProxyNodeBase>(B->GetOwningNode());


	if (A->LinkedTo.Num() > 0 || B->LinkedTo.Num() > 0)
	{
		return FPinConnectionResponse(CONNECT_RESPONSE_DISALLOW, TEXT("Too much connection"));
	}

	return FPinConnectionResponse(CONNECT_RESPONSE_MAKE, TEXT(""));
}

//UEdGraphSchema_MessageGraph......................................................................................................

UEdGraphSchema_MessageGraph::UEdGraphSchema_MessageGraph()
{
	SuitableDependetNodesType = EIncertNodeType::MessageGraphDependent;
	SuitableStandaloneNodesType = EIncertNodeType::MessageGraphStandalone;
}

