// Copyright 2016 Dmitriy Pavlov
#pragma once

#include "GraphEditorDragDropAction.h"

/** DragDropAction class for dropping a Variable onto a graph */
class FDragDropAction_StoryGraph : public FGraphEditorDragDropAction
{
public:
	DRAG_DROP_OPERATOR_TYPE(FDragDropAction_StoryGraph, FGraphEditorDragDropAction)

	// FGraphEditorDragDropAction interface
	virtual void HoverTargetChanged() override;
	virtual FReply DroppedOnPin(FVector2D ScreenPosition, FVector2D GraphPosition) override;
	virtual FReply DroppedOnNode(FVector2D ScreenPosition, FVector2D GraphPosition) override;
	virtual FReply DroppedOnPanel(const TSharedRef< class SWidget >& Panel, FVector2D ScreenPosition, FVector2D GraphPosition, UEdGraph& Graph) override;
	virtual FReply DroppedOnAction(TSharedRef<struct FEdGraphSchemaAction> Action) override;
	virtual FReply DroppedOnCategory(FText Category) override;
	// End of FGraphEditorDragDropAction

	static TSharedRef<FDragDropAction_StoryGraph> New(class UStoryGraphObject* Object)
	{
		TSharedRef<FDragDropAction_StoryGraph> Operation = MakeShareable(new FDragDropAction_StoryGraph);

		Operation->DraggedObject = Object;
		Operation->Construct();
		return Operation;
	}

	


protected:
	
	UStoryGraphObject* DraggedObject;

	
	struct FNodeConstructionParams
	{
		ENodeType NodeType;
		UStoryGraphObject* DraggedObject;
		FVector2D GraphPosition;
		UEdGraph* Graph;
	};


	static void SpawnNode(FNodeConstructionParams InParams);
	
};
