// Copyright 2016 Dmitriy Pavlov

#include "GraphPanelNodeFactory_StoryGraph.h"
#include "AssetEditor_StoryGraph.h"
#include "CustomNodes.h"
#include "Graph_StoryGraph.h"
#include "GraphNodes_StoryGraph.h"
#include "HUD_StoryGraph.h"
#include "ProxyNodes.h"
#include "ScopedTransaction.h"
#include "StoryGraph.h"


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
				BaseNode->CustomNode->PropertyUpdateDelegate.BindSP(
					((UEdGraph_StoryGraph*)BaseNode->GetGraph())->GetStoryGraph()->pAssetEditor,
					&FAssetEditor_StoryGraph::RefreshDetailPanel);

				return SNode;
			}

		case ENodeType::DialogNode:
		case ENodeType::Message:
			{
				TSharedPtr<SGraphNode_Dialog> SNode = SNew(SGraphNode_Dialog, BaseNode);
				BaseNode->CustomNode->NodeUpdateDelegate.BindSP(SNode.Get(),
				                                                &SGraphNode_CustomNodeBase::UpdateGraphNode);
				BaseNode->CustomNode->PropertyUpdateDelegate.BindSP(
					((UEdGraph_StoryGraph*)BaseNode->GetGraph())->GetStoryGraph()->pAssetEditor,
					&FAssetEditor_StoryGraph::RefreshDetailPanel);
				BaseNode->CustomNode->CreatePinDelegate.BindUObject(BaseNode, &UProxyNodeBase::HandleCreatePin);
				BaseNode->CustomNode->RemovePinDelegate.BindUObject(BaseNode, &UProxyNodeBase::HandleRemovePin);
				BaseNode->CustomNode->BreakPinDelegate.BindUObject(BaseNode, &UProxyNodeBase::HandleBreakPin);
				return SNode;
			}

		default:
			{
				switch (UCustomNodeBase::GetInsertNodeType(BaseNode->CustomNode->NodeType))
				{
				case EInsertNodeType::StoryGraphStandalone:

					{
						TSharedPtr<SGraphNode_CustomNodeBase> SNode = SNew(SGraphNode_CustomNodeBase, BaseNode);
						BaseNode->CustomNode->NodeUpdateDelegate.BindSP(SNode.Get(),
						                                                &SGraphNode_CustomNodeBase::UpdateGraphNode);
						BaseNode->CustomNode->PropertyUpdateDelegate.BindSP(
							((UEdGraph_StoryGraph*)BaseNode->GetGraph())->GetStoryGraph()->pAssetEditor,
							&FAssetEditor_StoryGraph::RefreshDetailPanel);

						return SNode;
					}
				case EInsertNodeType::StoryGraphDependent:

					{
						TSharedPtr<SGraphNode_StoryGraphDependedNode> SNode = SNew(
							SGraphNode_StoryGraphDependedNode, BaseNode);
						BaseNode->CustomNode->NodeUpdateDelegate.BindSP(SNode.Get(),
						                                                &SGraphNode_CustomNodeBase::UpdateGraphNode);
						BaseNode->CustomNode->PropertyUpdateDelegate.BindSP(
							((UEdGraph_StoryGraph*)BaseNode->GetGraph())->GetStoryGraph()->pAssetEditor,
							&FAssetEditor_StoryGraph::RefreshDetailPanel);

						return SNode;
					}
				case EInsertNodeType::DialogGraphStandalone:
				case EInsertNodeType::MessageGraphStandalone:
					{
						TSharedPtr<SGraphNode_DialogBase> SNode = SNew(SGraphNode_DialogBase, BaseNode);
						BaseNode->CustomNode->NodeUpdateDelegate.BindSP(SNode.Get(),
						                                                &SGraphNode_CustomNodeBase::UpdateGraphNode);
						BaseNode->CustomNode->PropertyUpdateDelegate.BindSP(
							((UEdGraph_StoryGraph*)BaseNode->GetGraph())->GetStoryGraph()->pAssetEditor,
							&FAssetEditor_StoryGraph::RefreshDetailPanel);
						return SNode;
					}
				case EInsertNodeType::DialogGraphDependent:
				case EInsertNodeType::MessageGraphDependent:
					{
						TSharedPtr<SGraphNode_DialogDependedNode> SNode = SNew(SGraphNode_DialogDependedNode, BaseNode);
						BaseNode->CustomNode->NodeUpdateDelegate.BindSP(SNode.Get(),
						                                                &SGraphNode_CustomNodeBase::UpdateGraphNode);
						BaseNode->CustomNode->PropertyUpdateDelegate.BindSP(
							((UEdGraph_StoryGraph*)BaseNode->GetGraph())->GetStoryGraph()->pAssetEditor,
							&FAssetEditor_StoryGraph::RefreshDetailPanel);
						return SNode;
					}
				}
			}
		}
	}
	return nullptr;
}
