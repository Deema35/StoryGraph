// Copyright 2016 Dmitriy Pavlov

#include "ProxyNods.h"
#include "CustomNods.h"
#include "EdGraph/EdGraphPin.h"
#include "EdGraph/EdGraph.h"
#include "AssetEditorManager.h"
#include "Graph_StoryGraph.h"
#include "StoryGraph.h"
#include "Developer/DesktopPlatform/Public/DesktopPlatformModule.h"


bool UProxyNodeBase::CycleInspection(uint32 UniqueID)
{
	for (int i = 0; i < Pins.Num(); i++)
	{
		if (Pins[i]->Direction == EEdGraphPinDirection::EGPD_Output)
		{
			for (int j = 0; j < Pins[i]->LinkedTo.Num(); j++)
			{
				if (Pins[i]->LinkedTo[j])
				{
					if (Pins[i]->LinkedTo[j]->GetOwningNode()->GetUniqueID() == UniqueID)
					{
						return false;
					}
					else
					{
						if (!Cast<UProxyNodeBase>(Pins[i]->LinkedTo[j]->GetOwningNode())->CycleInspection(UniqueID))
						{
							return false;
						}
					}
				}
			}
		}
	}
	return true;
}

FLinearColor UProxyNodeBase::GetNodeTitleColor() const
{
	return FLinearColor(0.7f, 0.7f, 0.7f);
}

FText UProxyNodeBase::GetTooltipText() const
{
	return FText::FromString(UCustomNodeBase::GetToolTipFromNodeType(CustomNode->NodeType));
}

FText UProxyNodeBase::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	
	return CustomNode->GetNodeTitle();
	
}


bool UProxyNodeBase::CanDuplicateNode() const
{
	return bUserDefined;
}

/** Whether or not this node can be deleted by user action */
bool UProxyNodeBase::CanUserDeleteNode() const
{
	return bUserDefined;
}

void UProxyNodeBase::PinConnectionListChanged(UEdGraphPin* Pin)
{
	Super::PinConnectionListChanged(Pin);

	for (int i = 0; i < Pins.Num(); i++)
	{
		if (Pins[i] == Pin)
		{
			CustomNode->NodePins[i].Links.Empty();
			for (int j = 0; j < Pins[i]->LinkedTo.Num(); j++)
			{
				CustomNode->NodePins[i].Links.Add(((UProxyNodeBase*)Pins[i]->LinkedTo[j]->GetOwningNode())->CustomNode);
			}
			CustomNode->PinConnectionListChanged(&CustomNode->NodePins[i]);
		}
	}

	CustomNode->pStoryGraph->StoryGraphState = EStoryGraphState::ST_Modify;
	
}

void UProxyNodeBase::PostLoad()
{
	Super::PostLoad();

	
	if (CustomNode) CustomNode->RefreshCollor();
}


void UProxyNodeBase::AllocateDefaultPins()
{
	for (int i = 0; i < CustomNode->NodePins.Num(); i++)
	{
		CreatePin((EEdGraphPinDirection)CustomNode->NodePins[i].Direction, UCustomNodeBase::GetPinDataTypeEnumAsString((EPinDataTypes)CustomNode->NodePins[i].PinDataType), TEXT(""), NULL, false, false, TEXT(""));
	}
}

void UProxyNodeBase::HandleCreatePin(FStoryGraphPin NewPin)
{
	CreatePin((EEdGraphPinDirection)NewPin.Direction, UCustomNodeBase::GetPinDataTypeEnumAsString((EPinDataTypes)NewPin.PinDataType), TEXT(""), NULL, false, false, TEXT(""));
}

void UProxyNodeBase::HandleRemovePin(int32 PinNumber)
{
	RemovePin(Pins[PinNumber]);
}

void UProxyNodeBase::HandleBreakPin(int32 PinNumber)
{
	Pins[PinNumber]->BreakAllPinLinks();
}

void UProxyNodeBase::DestroyNode()
{
	Super::DestroyNode();
	//Remove pins custom nod
	
	UEdGraph_StoryGraph* ParrentGraph = Cast<UEdGraph_StoryGraph>(GetGraph());

	if (UStoryGraph* StoryGraph = Cast<UStoryGraph>(ParrentGraph->GraphOwner))
	{
		StoryGraph->GarphNods.Remove(CustomNode);
			
	}
	else if (UStoryGraphCharecter* Charecter = Cast<UStoryGraphCharecter>(ParrentGraph->GraphOwner))
	{
		Charecter->GarphNods.Remove(CustomNode);
	}
	else if (UStoryGraphPlaceTrigger* PlaceTrigger = Cast<UStoryGraphPlaceTrigger>(ParrentGraph->GraphOwner))
	{
		PlaceTrigger->GarphNods.Remove(CustomNode);
	}
	
}