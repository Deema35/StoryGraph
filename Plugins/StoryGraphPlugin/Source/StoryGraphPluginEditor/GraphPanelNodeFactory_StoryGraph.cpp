// Copyright 2016 Dmitriy Pavlov

#include "GraphPanelNodeFactory_StoryGraph.h"
#include "ScopedTransaction.h"
#include "GraphNodes_StoryGraph.h"
#include "CustomNods.h"
#include "HUD_StoryGraph.h"
#include "AssetEditor_StoryGraph.h"
#include "Graph_StoryGraph.h"


FGraphPanelNodeFactory_StoryGraph::FGraphPanelNodeFactory_StoryGraph()
{
}

TSharedPtr<class SGraphNode> FGraphPanelNodeFactory_StoryGraph::CreateNode(UEdGraphNode* Node) const
{
	if (UProxyNodeBase* BaseNode = Cast<UProxyNodeBase>(Node))
	{
		switch (BaseNode->CustomNode->NodeType)
		{

		case ENodeType::DialogStart:
		case ENodeType::MessageStart:
		{
			TSharedPtr<SGraphNode_DialogRoot> SNode = SNew(SGraphNode_DialogRoot, BaseNode);
			BaseNode->CustomNode->NodeUpdateDelegate.BindSP(SNode.Get(), &SGraphNode_DialogRoot::UpdateGraphNode);
			BaseNode->CustomNode->ProprtyUpdateDelegate.BindSP(((UEdGraph_StoryGraph*)BaseNode->GetGraph())->GetStoryGraph()->pAssetEditor, &FAssetEditor_StoryGraph::RefreshDetailPanel);

			return SNode;
		}

		case ENodeType::DialogNode:
		case ENodeType::Message:
		{
			TSharedPtr<SGraphNode_Dialog> SNode = SNew(SGraphNode_Dialog, BaseNode);
			BaseNode->CustomNode->NodeUpdateDelegate.BindSP(SNode.Get(), &SGraphNode_CustomNodeBase::UpdateGraphNode);
			BaseNode->CustomNode->ProprtyUpdateDelegate.BindSP(((UEdGraph_StoryGraph*)BaseNode->GetGraph())->GetStoryGraph()->pAssetEditor, &FAssetEditor_StoryGraph::RefreshDetailPanel);
			BaseNode->CustomNode->CreatePinDelegate.BindUObject(BaseNode, &UProxyNodeBase::HandleCreatePin);
			BaseNode->CustomNode->RemovePinDelegate.BindUObject(BaseNode, &UProxyNodeBase::HandleRemovePin);
			BaseNode->CustomNode->BreakPinDelegate.BindUObject(BaseNode, &UProxyNodeBase::HandleBreakPin);
			return SNode;
		}

		default:
		{
			switch (UCustomNodeBase::GetIncertNodeType(BaseNode->CustomNode->NodeType))
			{

			case EIncertNodeType::StoryGraphStandalone:
			
			{
				TSharedPtr<SGraphNode_CustomNodeBase> SNode = SNew(SGraphNode_CustomNodeBase, BaseNode);
				BaseNode->CustomNode->NodeUpdateDelegate.BindSP(SNode.Get(), &SGraphNode_CustomNodeBase::UpdateGraphNode);
				BaseNode->CustomNode->ProprtyUpdateDelegate.BindSP(((UEdGraph_StoryGraph*)BaseNode->GetGraph())->GetStoryGraph()->pAssetEditor, &FAssetEditor_StoryGraph::RefreshDetailPanel);

				return SNode;
			}
			case  EIncertNodeType::StoryGraphDependent:
			
			{
				TSharedPtr<SGraphNode_StoryGraphDependetNode> SNode = SNew(SGraphNode_StoryGraphDependetNode, BaseNode);
				BaseNode->CustomNode->NodeUpdateDelegate.BindSP(SNode.Get(), &SGraphNode_CustomNodeBase::UpdateGraphNode);
				BaseNode->CustomNode->ProprtyUpdateDelegate.BindSP(((UEdGraph_StoryGraph*)BaseNode->GetGraph())->GetStoryGraph()->pAssetEditor, &FAssetEditor_StoryGraph::RefreshDetailPanel);

				return SNode;
			}
			case EIncertNodeType::DialogGraphStandalone:
			case EIncertNodeType::MessageGraphStandalone:
			{
				TSharedPtr<SGraphNode_DialogBase> SNode = SNew(SGraphNode_DialogBase, BaseNode);
				BaseNode->CustomNode->NodeUpdateDelegate.BindSP(SNode.Get(), &SGraphNode_CustomNodeBase::UpdateGraphNode);
				BaseNode->CustomNode->ProprtyUpdateDelegate.BindSP(((UEdGraph_StoryGraph*)BaseNode->GetGraph())->GetStoryGraph()->pAssetEditor, &FAssetEditor_StoryGraph::RefreshDetailPanel);
				return SNode;
			}
			case EIncertNodeType::DialogGraphDependent:
			case EIncertNodeType::MessageGraphDependent:
			{
				TSharedPtr<SGraphNode_DialogDependetNode> SNode = SNew(SGraphNode_DialogDependetNode, BaseNode);
				BaseNode->CustomNode->NodeUpdateDelegate.BindSP(SNode.Get(), &SGraphNode_CustomNodeBase::UpdateGraphNode);
				BaseNode->CustomNode->ProprtyUpdateDelegate.BindSP(((UEdGraph_StoryGraph*)BaseNode->GetGraph())->GetStoryGraph()->pAssetEditor, &FAssetEditor_StoryGraph::RefreshDetailPanel);
				return SNode;
			}

			}
			
			
		}
		}
	}
	return NULL;
}
