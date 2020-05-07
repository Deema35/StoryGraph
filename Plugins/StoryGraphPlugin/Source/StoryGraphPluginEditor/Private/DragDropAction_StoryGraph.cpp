// Copyright 2016 Dmitriy Pavlov

#include "DragDropAction_StoryGraph.h"
#include "CustomNodes.h"
#include "EditorStyle.h"
#include "GraphSchema_StoryGraph.h"
#include "StoryGraphObject.h"
#include "EdGraph/EdGraph.h"


void FDragDropAction_StoryGraph::HoverTargetChanged()
{
	UEdGraph* HoveredEdGraph = GetHoveredGraph();
	bool CanCreateNode = false;

	FSlateColor IconColor = FLinearColor::White;
	const FSlateBrush* StatusSymbol = FEditorStyle::GetBrush(TEXT("Graph.ConnectorFeedback.Error"));
	FText Message = FText::FromString("Cannot create node");

	if (HoveredEdGraph)
	{
		const UEdGraphSchema_Base* HoveredSchema = Cast<UEdGraphSchema_Base>(HoveredEdGraph->GetSchema());


		for (int i = 0; i < DraggedObject->DependedNodes.Num(); i++)
		{
			if (HoveredSchema && UCustomNodeBase::GetInsertNodeType(DraggedObject->DependedNodes[i]) == HoveredSchema->
				SuitableDependedNodesType)
			{
				CanCreateNode = true;
			}
		}
	}


	if (CanCreateNode)
	{
		StatusSymbol = FEditorStyle::GetBrush(TEXT("Graph.ConnectorFeedback.OK"));
		Message = FText::FromString("Can create node");
	}


	SetSimpleFeedbackMessage(StatusSymbol, IconColor, Message);
}

FReply FDragDropAction_StoryGraph::DroppedOnPin(FVector2D ScreenPosition, FVector2D GraphPosition)
{
	//UE_LOG(StoryGraphEditor, Warning, TEXT("Dropped On Pin"));

	return FReply::Handled();
}

FReply FDragDropAction_StoryGraph::DroppedOnNode(FVector2D ScreenPosition, FVector2D GraphPosition)
{
	//UE_LOG(StoryGraphEditor, Warning, TEXT("Dropped On Node"));

	return FReply::Unhandled();
}


FReply FDragDropAction_StoryGraph::DroppedOnPanel(const TSharedRef<SWidget>& Panel, FVector2D ScreenPosition,
                                                  FVector2D GraphPosition, UEdGraph& Graph)
{
	FNodeConstructionParams NewNodeParams;
	int MenuEntryCounter = 0;

	NewNodeParams.Graph = &Graph;
	NewNodeParams.DraggedObject = DraggedObject;
	NewNodeParams.GraphPosition = GraphPosition;

	const UEdGraphSchema_Base* GraphSchema = Cast<UEdGraphSchema_Base>(Graph.GetSchema());

	EInsertNodeType SuitableDependedNodesType = GraphSchema->SuitableDependedNodesType;


	FMenuBuilder MenuBuilder(true, nullptr);
	const FText VariableNameText = FText::FromString("Dependent nods");

	MenuBuilder.BeginSection("BPVariableDroppedOn", VariableNameText);

	for (int i = 0; i < DraggedObject->DependedNodes.Num(); i++)
	{
		if (UCustomNodeBase::GetInsertNodeType(DraggedObject->DependedNodes[i]) == SuitableDependedNodesType)
		{
			NewNodeParams.NodeType = DraggedObject->DependedNodes[i];

			FText MenuDesc = FText::FromString(UCustomNodeBase::GetActionNameFromNodeType(DraggedObject->DependedNodes[i]));
			FText ToolTip = FText::FromString(UCustomNodeBase::GetToolTipFromNodeType(DraggedObject->DependedNodes[i]));

			MenuBuilder.AddMenuEntry(
				MenuDesc,
				ToolTip,
				FSlateIcon(),
				FUIAction(FExecuteAction::CreateStatic(&FDragDropAction_StoryGraph::SpawnNode, NewNodeParams))
			);
			MenuEntryCounter++;
		}
	}

	if (MenuEntryCounter > 0)
	{
		TSharedRef<SWidget> PanelWidget = Panel;
		// Show dialog to choose getter vs setter
		FSlateApplication::Get().PushMenu(
			PanelWidget,
			FWidgetPath(),
			MenuBuilder.MakeWidget(),
			ScreenPosition,
			FPopupTransitionEffect(FPopupTransitionEffect::ContextMenu)
		);

		MenuBuilder.EndSection();
	}

	return FReply::Handled();
}


FReply FDragDropAction_StoryGraph::DroppedOnAction(TSharedRef<FEdGraphSchemaAction> Action)
{
	//UE_LOG(StoryGraphEditor, Warning, TEXT("Dropped On Action"));

	return FReply::Unhandled();
}

FReply FDragDropAction_StoryGraph::DroppedOnCategory(FText Category)
{
	//UE_LOG(StoryGraphEditor, Log, TEXT("Dropped on Category"));


	return FReply::Handled();
}


void FDragDropAction_StoryGraph::SpawnNode(FNodeConstructionParams InParams)
{
	FCustomSchemaAction_NewNode::SpawnNode(InParams.NodeType, InParams.DraggedObject, InParams.Graph, nullptr,
	                                       InParams.GraphPosition);
}
