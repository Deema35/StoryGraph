// Copyright 2016 Dmitriy Pavlov

#include "CustomNodes.h"
#include "Engine/Engine.h"
#include "HUD_StoryGraph.h"
#include "LogCategoryRutime.h"
#include "StoryGraph.h"
#include "StoryGraphObject.h"
#include "StoryGraphWidget.h"
#include "Public/OtherActor_StoryGraph.h"


//UCustomNodeBase...........................................................................................


void UCustomNodeBase::GetChildNodes(TArray<UCustomNodeBase*>& ChildNodes, EPinDataTypes OutPinType)
{
	ChildNodes.Empty();

	for (int i = 0; i < NodePins.Num(); i++)
	{
		if (NodePins[i].Direction == (int)EEdGraphPinDirection::EGPD_Output && NodePins[i].PinDataType == (int)
			OutPinType)
		{
			for (int j = 0; j < NodePins[i].Links.Num(); j++)
			{
				if (NodePins[i].Links[j])
				{
					ChildNodes.Add((UCustomNodeBase*)NodePins[i].Links[j]);
				}
			}
		}
	}
}

UCustomNodeBase* UCustomNodeBase::GetFistChildNode()
{
	TArray<UCustomNodeBase*> ChildNodes;

	GetChildNodes(ChildNodes, EPinDataTypes::PinType_Vertical);

	if (ChildNodes.Num() > 0)
	{
		return ChildNodes[0];
	}

	GetChildNodes(ChildNodes, EPinDataTypes::PinType_Horizontal);

	if (ChildNodes.Num() > 0)
	{
		return ChildNodes[0];
	}

	return nullptr;
}

void UCustomNodeBase::GetInputNodes(TArray<UCustomNodeBase*>& InputNodes, EPinDataTypes OutPinType)
{
	InputNodes.Empty();

	for (int i = 0; i < NodePins.Num(); i++)
	{
		if (NodePins[i].Direction == (int)EEdGraphPinDirection::EGPD_Input && NodePins[i].PinDataType == (int)OutPinType
		)
		{
			for (int j = 0; j < NodePins[i].Links.Num(); j++)
			{
				if (NodePins[i].Links[j])
				{
					InputNodes.Add((UCustomNodeBase*)NodePins[i].Links[j]);
				}
			}
		}
	}
}


FString UCustomNodeBase::GetPinDataTypeEnumAsString(EPinDataTypes EnumValue)
{
	const UEnum* EnumPtr = FindObject<UEnum>(ANY_PACKAGE, TEXT("EPinDataTypes"), true);
	if (!EnumPtr) return FString("Invalid");

	return EnumPtr->GetNameStringByIndex((int)EnumValue);
}


EInsertNodeType UCustomNodeBase::GetInsertNodeType(ENodeType NodeType)
{
	switch (NodeType)
	{
	case ENodeType::DialogNode:
	case ENodeType::DialogStart:
	case ENodeType::DialogEnd:
	case ENodeType::DialogExit:
	case ENodeType::AddDialogFromDialog:

		return EInsertNodeType::DialogGraphStandalone;

	case ENodeType::SetDialogTrigger:
	case ENodeType::SetInventoryItemState:

		return EInsertNodeType::DialogGraphDependent;

	case ENodeType::PrintString:
	case ENodeType::EndGame:
	case ENodeType::AddQuestPhase:
	case ENodeType::AddScreenMessage:
	case ENodeType::PrintQuestPhaseOnScreen:
	case ENodeType::SendMessageToLevelBlueprint:

		return EInsertNodeType::StoryGraphStandalone;

	case ENodeType::CancelQuest:
	case ENodeType::QuestStart:
	case ENodeType::GetStoryGraphObjectState:
	case ENodeType::AddDialog:
	case ENodeType::SetSceneObjectActive:
	case ENodeType::SendMessageScene:
	case ENodeType::AddTargetObjectToPhase:
	case ENodeType::AddMessageBranch:


		return EInsertNodeType::StoryGraphDependent;

	case ENodeType::MessageStart:
	case ENodeType::ActivateTrigger:
	case ENodeType::Message:
	case ENodeType::MessageExit:
	case ENodeType::MessageEnd:


		return EInsertNodeType::MessageGraphStandalone;

	case ENodeType::SetInventoryItemStateFromMessage:

		return EInsertNodeType::MessageGraphDependent;


	default:

		return EInsertNodeType::NotDefine;
	}
}

FString UCustomNodeBase::GetActionNameFromNodeType(ENodeType NodeType)
{
	switch (NodeType)
	{
	case ENodeType::PrintString:

		return "Print string";

	case ENodeType::DialogNode:

		return "Dialog node";

	case ENodeType::DialogStart:

		return "New dialog brunch";

	case ENodeType::DialogEnd:

		return "Dialog end";

	case ENodeType::DialogExit:

		return "Dialog exit";

	case ENodeType::SetDialogTrigger:

		return "Set dialog trigger";

	case ENodeType::AddQuestPhase:

		return "Add quest phase";

	case ENodeType::GetStoryGraphObjectState:

		return "Get object state";

	case ENodeType::AddDialog:

		return "Activate/deactivate dialog";

	case ENodeType::AddDialogFromDialog:

		return "Activate/deactivate dialog";

	case ENodeType::AddMessageBranch:

		return "Activate/deactivate message";

	case ENodeType::SetSceneObjectActive:

		return "Active/deactive object";

	case ENodeType::CancelQuest:

		return "Cancel quest";


	case ENodeType::QuestStart:

		return "Quest start";


	case ENodeType::EndGame:

		return "Game over";

	case ENodeType::SendMessageScene:

		return "Send Message";

	case ENodeType::AddScreenMessage:

		return "Add Screen Message";

	case ENodeType::AddTargetObjectToPhase:

		return "Add to radar";

	case ENodeType::MessageStart:

		return "New message brunch";

	case ENodeType::Message:

		return "Message";

	case ENodeType::MessageExit:

		return "Message exit";

	case ENodeType::MessageEnd:

		return "Message end";


	case ENodeType::ActivateTrigger:

		return "Activate trigger";

	case ENodeType::SetInventoryItemState:

		return "Set inventory state";

	case ENodeType::SetInventoryItemStateFromMessage:

		return "Set inventory state";

	case ENodeType::PrintQuestPhaseOnScreen:

		return "Print quest phase on screen";

	case ENodeType::SendMessageToLevelBlueprint:

		return "Send message to level blueprint";

	default:

		return "Not define";
	}
}

FString UCustomNodeBase::GetToolTipFromNodeType(ENodeType NodeType)
{
	switch (NodeType)
	{
	case ENodeType::PrintString:

		return "Print debug string on screen or log";

	case ENodeType::DialogNode:

		return "Add Dialog";

	case ENodeType::DialogStart:

		return "Create new dialog brunch";

	case ENodeType::DialogEnd:

		return "Set current dialog brunch unactive";

	case ENodeType::DialogExit:

		return "Exit from dialog screen";

	case ENodeType::SetDialogTrigger:

		return "Activate/Deactivate dialog trigger";

	case ENodeType::AddQuestPhase:

		return "Add quest phase";

	case ENodeType::GetStoryGraphObjectState:

		return "Waiting when object switch in current state";

	case ENodeType::AddDialog:

		return "Add or Remove dialog brunch for current character";

	case ENodeType::AddDialogFromDialog:

		return "Add or Remove dialog brunch for current dialog";

	case ENodeType::AddMessageBranch:

		return "Add or Remove message brunch for current place trigger";

	case ENodeType::SetSceneObjectActive:

		return "Active or deactive scene object";

	case ENodeType::QuestStart:

		return "Begin new quest from this node";


	case ENodeType::CancelQuest:

		return "Cancel quest performance";


	case ENodeType::EndGame:

		return "Mark current quest phase as last phase in game";

	case ENodeType::SendMessageScene:

		return "Send message to scene object";

	case ENodeType::AddScreenMessage:

		return "Add message on screen";

	case ENodeType::AddTargetObjectToPhase:

		return "Add object to current quest phase for it displayed on radar";

	case ENodeType::MessageStart:

		return "Create new message branch";

	case ENodeType::Message:

		return "Create new message node";


	case ENodeType::MessageExit:

		return "Exit from message screen mode";

	case ENodeType::MessageEnd:

		return "Set current message brunch unactive";

	case ENodeType::ActivateTrigger:

		return "Switch trigger in active state";

	case ENodeType::SetInventoryItemState:

		return "Set current state of inventory item";

	case ENodeType::SetInventoryItemStateFromMessage:

		return "Set current state of inventory item";

	case ENodeType::PrintQuestPhaseOnScreen:

		return "Print quest phase on screen";

	case ENodeType::SendMessageToLevelBlueprint:

		return "Send message to level blueprint";

	default:

		return "Not define";
	}
}

TSubclassOf<UCustomNodeBase> UCustomNodeBase::GetClassFromNodeType(ENodeType NodeType)
{
	switch (NodeType)
	{
	case ENodeType::PrintString:

		return UPrintStringNode::StaticClass();

	case ENodeType::DialogNode:

		return UDialogNode::StaticClass();

	case ENodeType::DialogStart:

		return UDialogStartNode::StaticClass();

	case ENodeType::DialogEnd:

		return UDialogEndNode::StaticClass();

	case ENodeType::DialogExit:

		return UDialogExitNode::StaticClass();

	case ENodeType::AddQuestPhase:

		return UAddQuestPhaseNode::StaticClass();

	case ENodeType::GetStoryGraphObjectState:

		return UGetStoryGraphObjectStateNode::StaticClass();

	case ENodeType::SetDialogTrigger:

		return USetDialogTriggerNode::StaticClass();

	case ENodeType::AddDialog:

		return UAddDialogNode::StaticClass();

	case ENodeType::AddDialogFromDialog:

		return UAddDialogFromDialogNode::StaticClass();

	case ENodeType::AddMessageBranch:

		return UAddMessageBranchNode::StaticClass();

	case ENodeType::SetSceneObjectActive:

		return USetSceneObjectActiveNode::StaticClass();

	case ENodeType::CancelQuest:

		return UCancelQuestNode::StaticClass();

	case ENodeType::QuestStart:

		return UQuestStartNode::StaticClass();

	case ENodeType::EndGame:

		return UEndGameNode::StaticClass();

	case ENodeType::SendMessageScene:

		return USendMessageNode::StaticClass();

	case ENodeType::AddScreenMessage:

		return UAddScreenMessageNode::StaticClass();

	case ENodeType::AddTargetObjectToPhase:

		return UAddTargetObjectToPhaseNode::StaticClass();

	case ENodeType::MessageStart:

		return UMessageStartNode::StaticClass();

	case ENodeType::Message:

		return UMessageNode::StaticClass();

	case ENodeType::MessageExit:

		return UMessageExitNode::StaticClass();

	case ENodeType::MessageEnd:

		return UMessageEndNode::StaticClass();

	case ENodeType::ActivateTrigger:

		return UActivateTriggerNode::StaticClass();

	case ENodeType::SetInventoryItemState:

		return USetInventoryItemStateNode::StaticClass();

	case ENodeType::SetInventoryItemStateFromMessage:

		return USetInventoryItemStateFromMessageNode::StaticClass();

	case ENodeType::PrintQuestPhaseOnScreen:

		return UPrintQuestPhaseOnScreenNode::StaticClass();

	case ENodeType::SendMessageToLevelBlueprint:

		return USendMessageToLevelBlueprintNode::StaticClass();

	default:

		return UCustomNodeBase::StaticClass();
	}
}

FName UCustomNodeBase::GetIconNameFromNodeType(ENodeType NodeType)
{
	switch (NodeType)
	{
	case ENodeType::AddTargetObjectToPhase:

		return FName("CustomNode.Radar");

	case ENodeType::QuestStart:

		return FName("CustomNode.QuestStart");

	default:

		return FName("Non");
	}
}

FLinearColor UCustomNodeBase::GetColorFromNodeType(ENodeType NodeType, int ColorNumber)
{
	switch (NodeType)
	{
	case ENodeType::AddDialog:

		if (ColorNumber == 0) return FLinearColor(0.1f, 0.1f, 0.4f);
		if (ColorNumber == 1) return FLinearColor(0.1f, 0.2f, 0.4f);

	case ENodeType::AddDialogFromDialog:

		return FLinearColor(0.1f, 0.2f, 0.4f);

	case ENodeType::AddMessageBranch:

		if (ColorNumber == 0) return FLinearColor(0.1f, 0.1f, 0.4f);
		if (ColorNumber == 1) return FLinearColor(0.1f, 0.2f, 0.4f);

	case ENodeType::AddQuestPhase:

		if (ColorNumber == 0) return FLinearColor(0.1f, 0.1f, 0.1f);
		if (ColorNumber == 1) return FLinearColor(0.4f, 0.1f, 0.1f);
		if (ColorNumber == 2) return FLinearColor(0.1f, 0.2f, 0.4f);

	case ENodeType::AddScreenMessage:

		return FLinearColor(0.4f, 0.1f, 0.4f);

	case ENodeType::AddTargetObjectToPhase:

		return FLinearColor(0.1f, 0.4f, 0.1f);

	case ENodeType::CancelQuest:

		return FLinearColor::Yellow;

	case ENodeType::DialogEnd:

		return FLinearColor(0.4f, 0.1f, 0.1f);

	case ENodeType::DialogExit:

		return FLinearColor(0.4f, 0.1f, 0.1f);

	case ENodeType::DialogNode:

		if (ColorNumber == 0) return FLinearColor(0.1f, 0.1f, 0.1f);
		if (ColorNumber == 1) return FLinearColor(0.1f, 0.4f, 0.1f);
		if (ColorNumber == 2) return FLinearColor(0.1f, 0.1f, 0.4f);

	case ENodeType::DialogStart:

		if (ColorNumber == 0) return FLinearColor(0.1f, 0.1f, 0.1f);
		if (ColorNumber == 1) return FLinearColor(0.1f, 0.1f, 0.4f);

	case ENodeType::EndGame:

		return FLinearColor(0.4f, 0.1f, 0.1f);

	case ENodeType::GetStoryGraphObjectState:

		return FLinearColor(0.4f, 0.2f, 0.0f);

	case ENodeType::PrintString:

		return FLinearColor::Yellow;

	case ENodeType::QuestStart:

		if (ColorNumber == 0) return FLinearColor(0.4f, 0.1f, 0.1f);
		if (ColorNumber == 1) return FLinearColor(0.1f, 0.2f, 0.4f);

	case ENodeType::SendMessageScene:

		return FLinearColor(0.1f, 0.4f, 0.1f);

	case ENodeType::SetDialogTrigger:

		return FLinearColor(0.4f, 0.2f, 0.0f);

	case ENodeType::SetSceneObjectActive:

		return FLinearColor(0.1f, 0.4f, 0.1f);

	case ENodeType::MessageStart:

		return FLinearColor(0.1f, 0.1f, 0.4f);

	case ENodeType::Message:

		if (ColorNumber == 0) return FLinearColor(0.1f, 0.1f, 0.1f);
		if (ColorNumber == 1) return FLinearColor(0.1f, 0.4f, 0.1f);
		if (ColorNumber == 2) return FLinearColor(0.1f, 0.1f, 0.4f);

	case ENodeType::MessageExit:

		return FLinearColor(0.4f, 0.1f, 0.1f);

	case ENodeType::MessageEnd:

		return FLinearColor(0.4f, 0.1f, 0.1f);

	case ENodeType::ActivateTrigger:

		return FLinearColor(0.4f, 0.2f, 0.0f);

	case ENodeType::SetInventoryItemState:

		return FLinearColor(0.1f, 0.4f, 0.1f);

	case ENodeType::SetInventoryItemStateFromMessage:

		return FLinearColor(0.1f, 0.4f, 0.1f);

	case ENodeType::PrintQuestPhaseOnScreen:

		return FLinearColor(0.1f, 0.4f, 0.1f);

	case ENodeType::SendMessageToLevelBlueprint:

		return FLinearColor(0.1f, 0.4f, 0.1f);

	default:

		return FLinearColor(0.1f, 0.1f, 0.1f);
	}
}

EPerformNodeResult UCustomNodeBase::PerformNode()
{
	return EPerformNodeResult::Successed;
}

FText UCustomNodeBase::GetNodeTitle() const
{
	return FText::FromString(GetActionNameFromNodeType(NodeType));
}
#if WITH_EDITORONLY_DATA

void UCustomNodeBase::InitNode(UStoryGraphObject* pGraphObject_)
{
	pGraphObject = pGraphObject_;
	RefreshColor();
}


void UCustomNodeBase::CreatePin(FStoryGraphPin NewPin)
{
	NodePins.Add(NewPin);
	CreatePinDelegate.ExecuteIfBound(NewPin);
	UpdateGraphNode();
}

void UCustomNodeBase::RemovePin(int32 PinNumber)
{
	NodePins.RemoveAt(PinNumber);
	RemovePinDelegate.ExecuteIfBound(PinNumber);
	UpdateGraphNode();
}

void UCustomNodeBase::UpdateGraphNode()
{
	NodeUpdateDelegate.ExecuteIfBound();
	pStoryGraph->StoryGraphState = EStoryGraphState::ST_Modify;
}

void UCustomNodeBase::PostEditChangeProperty(struct FPropertyChangedEvent& e)
{
	Super::PostEditChangeProperty(e);
	UpdateGraphNode();
}

void UCustomNodeBase::GetXMLSavingProperty(std::map<FString, XMLProperty>& Properties)
{
	Properties.clear();

	Properties.insert(
		std::pair<FString, XMLProperty
		>("NodeType", XMLProperty(GetEnumValueAsString<ENodeType>("ENodeType", NodeType))));

	if (pGraphObject)
	{
		Properties.insert(std::pair<FString, XMLProperty>("GraphOject", XMLProperty(pGraphObject->XMLID)));
	}
	else
	{
		Properties.insert(std::pair<FString, XMLProperty>("GraphOject", XMLProperty("Non")));
	}
	Properties.insert(std::pair<FString, XMLProperty>("Comment", XMLProperty(Comment)));


	//Add links to another nods

	Properties.insert(std::pair<FString, XMLProperty>("Arr_Pins", XMLProperty("")));
	XMLProperty& PinsPointer = Properties["Arr_Pins"];

	for (int i = 0; i < NodePins.Num(); i++)
	{
		PinsPointer.Properties.insert(
			std::pair<FString, XMLProperty>("Arr_Pin" + FString::FromInt(i), XMLProperty("")));
		XMLProperty& LinksPointer = PinsPointer.Properties["Arr_Pin" + FString::FromInt(i)];
		if (NodePins[i].Direction == (int)EEdGraphPinDirection::EGPD_Output)
		{
			for (int j = 0; j < NodePins[i].Links.Num(); j++)
			{
				LinksPointer.Properties.insert(std::pair<FString, XMLProperty>(
					"Link" + FString::FromInt(i) + FString::FromInt(j),
					XMLProperty(NodePins[i].Links[j]->XMLID)));
			}
		}
	}
}

void UCustomNodeBase::LoadPropertyFromXML(std::map<FString, XMLProperty>& Properties)
{
	Comment = Properties["Comment"].Val;
	RefreshColor();
}

void UCustomNodeBase::DeleteLinkToNode(UCustomNodeBase* NodeLink)
{
	for (int i = 0; i < NodePins.Num(); i++)
	{
		for (int j = 0; j < NodePins[i].Links.Num(); j++)
		{
			if (NodePins[i].Links[j] == NodeLink)
			{
				NodePins[i].Links.RemoveAt(j);
				return;
			}
		}
	}
}
#endif //WITH_EDITORONLY_DATA

//UDialogNodeBase...........................................................................................
FText UDialogNodeBase::GetDialogName(FText Dialog)
{
	return FText::FromString(Dialog.ToString().Left(18));
}

bool UDialogNodeBase::GetChildNode(const UDialogObject* CurrentDialog, UDialogNodeBase*& NextNode)
{
	if (!CurrentDialog)
	{
		NextNode = (UDialogNodeBase*)GetFistChildNode();
		return true;
	}
	else
	{
		for (int j = 0; j < NodePins[CurrentDialog->CurrentDialogPin].Links.Num(); j++)
		{
			if (NodePins[CurrentDialog->CurrentDialogPin].Links[j])
			{
				NextNode = (UDialogNodeBase*)NodePins[CurrentDialog->CurrentDialogPin].Links[j];

				return true;
			}
		}
	}

	return false;
}
#if WITH_EDITORONLY_DATA
void UDialogNodeBase::RefreshDialogOwner()
{
	TArray<UCustomNodeBase*> InputNodes;

	GetInputNodes(InputNodes, EPinDataTypes::PinType_Horizontal);

	if (InputNodes.Num() > 0)
	{
		UDialogNodeBase* AnotherDialogNodeBase = Cast<UDialogNodeBase>(InputNodes[0]);

		if (AnotherDialogNodeBase->DialogOwner == ECharDialogOwner::NotDefine)
		{
			TopDialog = nullptr;
			DialogOwner = ECharDialogOwner::NotDefine;
		}
		else if (AnotherDialogNodeBase->DialogOwner == ECharDialogOwner::Character)
		{
			TopDialog = AnotherDialogNodeBase->TopDialog;

			DialogOwner = ECharDialogOwner::Character;
		}
		else if (AnotherDialogNodeBase->DialogOwner == ECharDialogOwner::NPC)
		{
			TopDialog = AnotherDialogNodeBase->TopDialog;
			DialogOwner = ECharDialogOwner::NPC;
		}
	}
	else
	{
		TopDialog = nullptr;
		DialogOwner = ECharDialogOwner::NotDefine;
	}

	UpdateGraphNode();

	TArray<UCustomNodeBase*> ChildNodes;

	GetChildNodes(ChildNodes, EPinDataTypes::PinType_Horizontal);


	for (int j = 0; j < ChildNodes.Num(); j++)
	{
		((UDialogNodeBase*)ChildNodes[j])->RefreshDialogOwner();
	}
}

void UDialogNodeBase::PinConnectionListChanged(FStoryGraphPin* Pin)
{
	Super::PinConnectionListChanged(Pin);

	if (Pin->Direction == EEdGraphPinDirection::EGPD_Input)
	{
		RefreshDialogOwner();
	}
}

void UDialogNodeBase::AddDialog()
{
	UDialogObject* NewDialog = NewObject<UDialogObject>(this, UDialogObject::StaticClass());
	NewDialog->DialogNode = this;
	NewDialog->Dialog = FText::FromString("New dialog");

	NewDialog->CurrentDialogPin = Dialogs.Add(NewDialog) + 1;
	CreatePin(FStoryGraphPin(EGPD_Output, EPinDataTypes::PinType_Horizontal));
	UpdateGraphNode(); //Update graph node
}

void UDialogNodeBase::GetXMLSavingProperty(std::map<FString, XMLProperty>& Properties)
{
	Super::GetXMLSavingProperty(Properties);


	Properties.insert(std::pair<FString, XMLProperty>("Arr_Dialogs", XMLProperty("")));
	XMLProperty& DialogsPointer = Properties["Arr_Dialogs"];

	for (int i = 0; i < Dialogs.Num(); i++)
	{
		DialogsPointer.Properties.insert(
			std::pair<FString, XMLProperty>("Dialog_" + FString::FromInt(i),
			                                XMLProperty(Dialogs[i]->Dialog.ToString())));
	}
}

void UDialogNodeBase::LoadPropertyFromXML(std::map<FString, XMLProperty>& Properties)
{
	if (Properties["Arr_Dialogs"].Properties.size() > 1)
	{
		for (int i = 0; i < Properties["Arr_Dialogs"].Properties.size() - 1; i++)
		{
			AddDialog();
		}
	}

	int i = 0;
	for (auto it = Properties["Arr_Dialogs"].Properties.begin(); it != Properties["Arr_Dialogs"].Properties.end(); ++it)
	{
		Dialogs[i++]->Dialog = FText::FromString(it->second.Val);
	}

	Super::LoadPropertyFromXML(Properties);
}
#endif //WITH_EDITORONLY_DATA

//UStoryVerticalNodeBase...........................................................................................

void UStoryVerticalNodeBase::ResetUnPerformBrunch()
{
	TArray<UCustomNodeBase*> InputNodes;

	GetInputNodes(InputNodes, EPinDataTypes::PinType_Vertical);
	bPerformNode = true;
	for (int i = 0; i < InputNodes.Num(); i++)
	{
		if (!((UStoryVerticalNodeBase*)InputNodes[i])->bPerformNode)
		{
			((UStoryVerticalNodeBase*)InputNodes[i])->ResetUnPerformBrunch();
		}
	}
}

EPerformNodeResult UStoryVerticalNodeBase::PerformNode()
{
	Super::PerformNode();
	TArray<UCustomNodeBase*> ChildNodes;

	GetChildNodes(ChildNodes, EPinDataTypes::PinType_Horizontal);

	for (int i = 0; i < ChildNodes.Num(); i++)
	{
		(ChildNodes[i])->PerformNode();
	}

	return EPerformNodeResult::Successed;
}
#if WITH_EDITORONLY_DATA
void UStoryVerticalNodeBase::RefreshQuestOwner()
{
	TArray<UCustomNodeBase*> InputNodes;

	GetInputNodes(InputNodes, EPinDataTypes::PinType_Vertical);


	if (InputNodes.Num() > 0)
	{
		UStoryVerticalNodeBase* AnotherVerticalNodeBase = Cast<UStoryVerticalNodeBase>(InputNodes[0]);
		//Refresh quest phase

		pQuestPhase = AnotherVerticalNodeBase->pQuestPhase;

		//Refresh Quest owner
		pQuestOwner = AnotherVerticalNodeBase->pQuestOwner;
	}
	else
	{
		pQuestOwner = nullptr;

		pQuestPhase = nullptr;
	}

	RefreshColor();
	UpdateGraphNode();

	TArray<UCustomNodeBase*> ChildNodes;

	GetChildNodes(ChildNodes, EPinDataTypes::PinType_Vertical);

	for (int i = 0; i < ChildNodes.Num(); i++)
	{
		((UStoryVerticalNodeBase*)ChildNodes[i])->RefreshQuestOwner();
	}
}


void UStoryVerticalNodeBase::PinConnectionListChanged(FStoryGraphPin* Pin)
{
	Super::PinConnectionListChanged(Pin);

	if (Pin->Direction == (int)EEdGraphPinDirection::EGPD_Input && Pin->PinDataType == (int)EPinDataTypes::
		PinType_Vertical)
	{
		RefreshQuestOwner();
	}
}
#endif //WITH_EDITORONLY_DATA
//UStoryHorizontalNodeBase...........................................................................................

EPerformNodeResult UStoryHorizontalNodeBase::PerformNode()
{
	Super::PerformNode();

	TArray<UCustomNodeBase*> ChildNodes;

	GetChildNodes(ChildNodes, EPinDataTypes::PinType_Horizontal);

	for (int i = 0; i < ChildNodes.Num(); i++)
	{
		(ChildNodes[i])->PerformNode();
	}

	return EPerformNodeResult::Successed;
}
#if WITH_EDITORONLY_DATA
void UStoryHorizontalNodeBase::RefreshQuestPhase()
{
	TArray<UCustomNodeBase*> InputNodes;

	GetInputNodes(InputNodes, EPinDataTypes::PinType_Horizontal);


	if (InputNodes.Num() > 0) //we have input links
	{
		UStoryVerticalNodeBase* VerticalNode = Cast<UStoryVerticalNodeBase>(InputNodes[0]);
		UStoryHorizontalNodeBase* HorizontalNode = Cast<UStoryHorizontalNodeBase>(InputNodes[0]);
		if (VerticalNode)
		{
			pQuestPhase = VerticalNode->pQuestPhase;
		}
		else if (HorizontalNode)
		{
			pQuestPhase = HorizontalNode->pQuestPhase;
		}
	}
	else
	{
		pQuestPhase = nullptr;
	}
	//Update out nods
	TArray<UCustomNodeBase*> ChildNodes;

	GetChildNodes(ChildNodes, EPinDataTypes::PinType_Horizontal);

	for (int j = 0; j < ChildNodes.Num(); j++)
	{
		((UStoryHorizontalNodeBase*)ChildNodes[j])->RefreshQuestPhase();
	}

	OwnedQuestPhase = pQuestPhase ? UDialogNodeBase::GetDialogName(pQuestPhase->Description).ToString() : "Non";
}


void UStoryHorizontalNodeBase::PinConnectionListChanged(FStoryGraphPin* Pin)
{
	Super::PinConnectionListChanged(Pin);

	if (Pin->Direction == (int)EEdGraphPinDirection::EGPD_Input && Pin->PinDataType == (int)EPinDataTypes::
		PinType_Horizontal)
	{
		RefreshQuestPhase();
	}
}
#endif //WITH_EDITORONLY_DATA
//UAddQuestPhaseNode...........................................................................................

UAddQuestPhaseNode::UAddQuestPhaseNode()
{
	NodeType = ENodeType::AddQuestPhase;

	NodePins.Add(FStoryGraphPin(EEdGraphPinDirection::EGPD_Input, EPinDataTypes::PinType_Vertical));
	NodePins.Add(FStoryGraphPin(EEdGraphPinDirection::EGPD_Output, EPinDataTypes::PinType_Horizontal));
	NodePins.Add(FStoryGraphPin(EEdGraphPinDirection::EGPD_Output, EPinDataTypes::PinType_Vertical));
}


EPerformNodeResult UAddQuestPhaseNode::PerformNode()
{
	Super::PerformNode();

	if (bPerformNode)
	{
		return EPerformNodeResult::NodeAlreadyPerformed;
	}
	else
	{
		if (!IsEmpty)
		{
			(QuestPhaseToAdd->pOwnedQuest)->AddPhase(QuestPhaseToAdd);
		}
		bPerformNode = true;
		return EPerformNodeResult::Successed;
	}
}

FText UAddQuestPhaseNode::GetNodeTitle() const
{
	if (IsEmpty)
	{
		return FText::FromString("Empty phase");
	}
	return UDialogNodeBase::GetDialogName(QuestPhaseToAdd->Description);
}
#if WITH_EDITORONLY_DATA

void UAddQuestPhaseNode::InitNode(UStoryGraphObject* pGraphObject_)
{
	Super::InitNode(pGraphObject_);

	QuestPhaseToAdd = NewObject<UQuestPhase>(this, UQuestPhase::StaticClass());
	QuestPhaseToAdd->Description = FText::FromString("New Quest Phase");
	pQuestPhase = QuestPhaseToAdd;
}

void UAddQuestPhaseNode::SetQuestPhase(FText NewQuestPhase)
{
	QuestPhaseToAdd->Description = NewQuestPhase;

	UpdateGraphNode();
}

void UAddQuestPhaseNode::RefreshColor()
{
	if (pQuestOwner)
	{
		if (pQuestOwner->MainQuest)
		{
			NodeColor = UCustomNodeBase::GetColorFromNodeType(ENodeType::AddQuestPhase, 1);
		}
		else
		{
			NodeColor = UCustomNodeBase::GetColorFromNodeType(ENodeType::AddQuestPhase, 2);
		}
	}
	else
	{
		NodeColor = UCustomNodeBase::GetColorFromNodeType(ENodeType::AddQuestPhase, 0);
	}
}

void UAddQuestPhaseNode::PinConnectionListChanged(FStoryGraphPin* Pin)
{
	Super::PinConnectionListChanged(Pin);

	PropertyUpdateDelegate.ExecuteIfBound();
}

void UAddQuestPhaseNode::RefreshQuestOwner()
{
	if (IsEmpty)
	{
		Super::RefreshQuestOwner();
		RefreshColor();
		UpdateGraphNode();
		return;
	}

	//Refresh quest phase
	pQuestPhase = QuestPhaseToAdd;

	TArray<UCustomNodeBase*> InputNodes;

	GetInputNodes(InputNodes, EPinDataTypes::PinType_Vertical);

	if (InputNodes.Num() > 0)
	{
		UStoryVerticalNodeBase* AnotherVerticalNode = Cast<UStoryVerticalNodeBase>(InputNodes[0]);

		//Refresh Quest owner
		pQuestOwner = AnotherVerticalNode->pQuestOwner;
	}
	else
	{
		pQuestOwner = nullptr;
	}

	QuestPhaseToAdd->pOwnedQuest = pQuestOwner;

	RefreshColor();
	UpdateGraphNode();


	//Update out nods
	TArray<UCustomNodeBase*> ChildNodes;

	GetChildNodes(ChildNodes, EPinDataTypes::PinType_Vertical);

	for (int i = 0; i < ChildNodes.Num(); i++)
	{
		((UStoryVerticalNodeBase*)ChildNodes[i])->RefreshQuestOwner();
	}
}

void UAddQuestPhaseNode::GetXMLSavingProperty(std::map<FString, XMLProperty>& Properties)
{
	Super::GetXMLSavingProperty(Properties);

	Properties.insert(
		std::pair<FString, XMLProperty>("QuestPhaseToAdd", XMLProperty(QuestPhaseToAdd->Description.ToString())));
	Properties.insert(std::pair<FString, XMLProperty>("IsEmpty", XMLProperty(IsEmpty ? "true" : "false")));
}

void UAddQuestPhaseNode::LoadPropertyFromXML(std::map<FString, XMLProperty>& Properties)
{
	QuestPhaseToAdd->Description = FText::FromString(Properties["QuestPhaseToAdd"].Val);
	IsEmpty = Properties["IsEmpty"].Val == "true";

	Super::LoadPropertyFromXML(Properties);
}
#endif //WITH_EDITORONLY_DATA
//UDialogStartNode...........................................................................................

UDialogStartNode::UDialogStartNode()
{
	NodeType = ENodeType::DialogStart;
	DialogOwner = ECharDialogOwner::Character;

	IsActive = true;
	NodePins.Add(FStoryGraphPin(EEdGraphPinDirection::EGPD_Output, EPinDataTypes::PinType_Horizontal));
}
#if WITH_EDITORONLY_DATA
void UDialogStartNode::InitNode(UStoryGraphObject* pGraphObject_)
{
	Super::InitNode(pGraphObject_);

	Dialogs.Add(NewObject<UDialogObject>(this, UDialogObject::StaticClass()));
	Dialogs[0]->DialogNode = this;
	Dialogs[0]->Dialog = FText::FromString("Enter text");
	Dialogs[0]->CurrentDialogPin = 0;
	TopDialog = Dialogs[0];
}

void UDialogStartNode::SetNewDialog(FText NewDialog)
{
	Dialogs[0]->Dialog = NewDialog;

	UpdateGraphNode(); //Refresh node

	for (int i = 0; i < pStoryGraph->GraphNodes.Num(); i++)
	{
		if (pStoryGraph->GraphNodes[i]->NodeType == ENodeType::AddDialog)
		{
			pStoryGraph->GraphNodes[i]->UpdateGraphNode(); //Refresh all USetQuestStateNode nodes
		}
	}
}

void UDialogStartNode::RefreshColor()
{
	Super::RefreshColor();

	NodeColor = IsActive
		            ? UCustomNodeBase::GetColorFromNodeType(ENodeType::DialogStart, 1)
		            : UCustomNodeBase::GetColorFromNodeType(ENodeType::DialogStart, 0);
	UpdateGraphNode();
}

void UDialogStartNode::RefreshDialogOwner()
{
	if (!TopDialog)
	{
		TopDialog = Dialogs[0];
	}

	Super::RefreshDialogOwner();
}

void UDialogStartNode::GetXMLSavingProperty(std::map<FString, XMLProperty>& Properties)
{
	Super::GetXMLSavingProperty(Properties);

	Properties.insert(std::pair<FString, XMLProperty>("IsActive", IsActive ? "true" : "false"));
	Properties.insert(std::pair<FString, XMLProperty>("DialogPriority", XMLProperty(FString::FromInt(DialogPriority))));
}

void UDialogStartNode::LoadPropertyFromXML(std::map<FString, XMLProperty>& Properties)
{
	IsActive = Properties["IsActive"].Val == "true";
	DialogPriority = FCString::Atoi(*Properties["DialogPriority"].Val);

	Super::LoadPropertyFromXML(Properties);
}
#endif //WITH_EDITORONLY_DATA
//UDialogNode...........................................................................................

UDialogNode::UDialogNode()
{
	NodeType = ENodeType::DialogNode;
	TopDialog = nullptr;
	DialogOwner = ECharDialogOwner::NotDefine;

	NodePins.Add(FStoryGraphPin(EEdGraphPinDirection::EGPD_Input, EPinDataTypes::PinType_Horizontal));
	NodePins.Add(FStoryGraphPin(EEdGraphPinDirection::EGPD_Output, EPinDataTypes::PinType_Horizontal));
}

#if WITH_EDITORONLY_DATA
void UDialogNode::InitNode(UStoryGraphObject* pGraphObject_)
{
	Super::InitNode(pGraphObject_);

	Dialogs.Add(NewObject<UDialogObject>(this, UDialogObject::StaticClass()));
	Dialogs[0]->DialogNode = this;
	Dialogs[0]->Dialog = FText::FromString("Enter text");
	Dialogs[0]->CurrentDialogPin = 1;
}

void UDialogNode::RefreshColor()
{
	TArray<UCustomNodeBase*> InputNodes;

	GetInputNodes(InputNodes, EPinDataTypes::PinType_Horizontal);


	if (InputNodes.Num() > 0)
	{
		UDialogNodeBase* AnotherDialogNodeBase = Cast<UDialogNodeBase>(InputNodes[0]);

		if (AnotherDialogNodeBase->DialogOwner == ECharDialogOwner::Character)
		{
			NodeColor = UCustomNodeBase::GetColorFromNodeType(ENodeType::DialogNode, 1);
		}
		else if (AnotherDialogNodeBase->DialogOwner == ECharDialogOwner::NPC)
		{
			NodeColor = UCustomNodeBase::GetColorFromNodeType(ENodeType::DialogNode, 2);
		}
	}
	else
	{
		NodeColor = UCustomNodeBase::GetColorFromNodeType(ENodeType::DialogNode, 0);
	}
}

void UDialogNode::RefreshDialogOwner()
{
	TArray<UCustomNodeBase*> InputNodes;

	GetInputNodes(InputNodes, EPinDataTypes::PinType_Horizontal);

	if (InputNodes.Num() > 0)
	{
		UDialogNodeBase* AnotherDialogNodeBase = Cast<UDialogNodeBase>(InputNodes[0]);
		if (AnotherDialogNodeBase->DialogOwner == ECharDialogOwner::NotDefine)
		{
			TopDialog = nullptr;
			DialogOwner = ECharDialogOwner::NotDefine;
		}
		else if (AnotherDialogNodeBase->DialogOwner == ECharDialogOwner::Character)
		{
			TopDialog = AnotherDialogNodeBase->TopDialog;
			DialogOwner = ECharDialogOwner::NPC;
		}
		else if (AnotherDialogNodeBase->DialogOwner == ECharDialogOwner::NPC)
		{
			TopDialog = AnotherDialogNodeBase->TopDialog;
			DialogOwner = ECharDialogOwner::Character;
		}
	}
	else
	{
		TopDialog = nullptr;
		DialogOwner = ECharDialogOwner::NotDefine;
	}
	PropertyUpdateDelegate.ExecuteIfBound();

	//Remove links from hidden pins
	UCustomNodeBase* LinkToNode;

	if (DialogOwner == ECharDialogOwner::NPC)
	{
		for (int i = 2; i < NodePins.Num(); i++)
		{
			LinkToNode = nullptr;
			if (NodePins[i].Links.Num() > 0) LinkToNode = NodePins[i].Links[0];
			BreakPinDelegate.ExecuteIfBound(i);

			if (LinkToNode) ((UDialogNodeBase*)LinkToNode)->RefreshDialogOwner();
		}
	}


	RefreshColor();
	UpdateGraphNode();

	//Update out nods
	TArray<UCustomNodeBase*> ChildNodes;

	GetChildNodes(ChildNodes, EPinDataTypes::PinType_Horizontal);

	for (int j = 0; j < ChildNodes.Num(); j++)
	{
		((UDialogNodeBase*)ChildNodes[j])->RefreshDialogOwner();
	}
}
#endif //WITH_EDITORONLY_DATA
//UDialogEndNode...................................................................

UDialogEndNode::UDialogEndNode()
{
	NodeType = ENodeType::DialogEnd;
	NodePins.Add(FStoryGraphPin(EEdGraphPinDirection::EGPD_Input, EPinDataTypes::PinType_Horizontal));
	NodePins.Add(FStoryGraphPin(EEdGraphPinDirection::EGPD_Output, EPinDataTypes::PinType_Horizontal));
}

EPerformNodeResult UDialogEndNode::PerformNode()
{
	Super::PerformNode();
	if (TopDialog)
	{
		((UDialogStartNode*)TopDialog->DialogNode)->IsActive = false;
	}
	else
	{
		UE_LOG(LogCategoryStoryGraphPluginRuntime, Warning, TEXT("Top dialog non"));
	}
	return EPerformNodeResult::Successed;
}

//UDialogExitNode...................................................................

UDialogExitNode::UDialogExitNode()
{
	NodeType = ENodeType::DialogExit;

	NodePins.Add(FStoryGraphPin(EEdGraphPinDirection::EGPD_Input, EPinDataTypes::PinType_Horizontal));
}

EPerformNodeResult UDialogExitNode::PerformNode()
{
	Super::PerformNode();
	if (pStoryGraph && pStoryGraph->OwnedActor)
	{
		AHUD_StoryGraph* HUD = Cast<AHUD_StoryGraph>(
			pStoryGraph->OwnedActor->GetWorld()->GetFirstPlayerController()->GetHUD());

		if (HUD)
		{
			HUD->OpenDialogEvent(false);
		}
		else
		{
			UE_LOG(LogCategoryStoryGraphPluginRuntime, Warning, TEXT("Your HUD should inherit AHUD_StoryGraph class"));
		}
	}
	return EPerformNodeResult::Successed;
}

//USetDialogTriggerNode............................................................................

USetDialogTriggerNode::USetDialogTriggerNode()
{
	NodeType = ENodeType::SetDialogTrigger;

	NodePins.Add(FStoryGraphPin(EEdGraphPinDirection::EGPD_Input, EPinDataTypes::PinType_Horizontal));
	NodePins.Add(FStoryGraphPin(EEdGraphPinDirection::EGPD_Output, EPinDataTypes::PinType_Horizontal));
}

EPerformNodeResult USetDialogTriggerNode::PerformNode()
{
	Super::PerformNode();

	pGraphObject->SetCurrentState((int)TriggerState);
	return EPerformNodeResult::Successed;
}

FText USetDialogTriggerNode::GetNodeTitle() const
{
	return FText::FromString(GetEnumValueAsString<EDialogTriggerStates>("EDialogTriggerStates", TriggerState));
}

#if WITH_EDITORONLY_DATA

void USetDialogTriggerNode::GetXMLSavingProperty(std::map<FString, XMLProperty>& Properties)
{
	Super::GetXMLSavingProperty(Properties);

	Properties.insert(
		std::pair<FString, XMLProperty>("TriggerState", XMLProperty(FString::FromInt((int)TriggerState))));
}

void USetDialogTriggerNode::LoadPropertyFromXML(std::map<FString, XMLProperty>& Properties)
{
	TriggerState = (EDialogTriggerStates)FCString::Atoi(*Properties["TriggerState"].Val);

	Super::LoadPropertyFromXML(Properties);
}

#endif //WITH_EDITORONLY_DATA

//USetSceneObjectActiveNode...................................................................

USetSceneObjectActiveNode::USetSceneObjectActiveNode()
{
	NodeType = ENodeType::SetSceneObjectActive;

	NodePins.Add(FStoryGraphPin(EEdGraphPinDirection::EGPD_Input, EPinDataTypes::PinType_Horizontal));
	NodePins.Add(FStoryGraphPin(EEdGraphPinDirection::EGPD_Output, EPinDataTypes::PinType_Horizontal));
}

EPerformNodeResult USetSceneObjectActiveNode::PerformNode()
{
	Super::PerformNode();

	((UStoryGraphObjectWithSceneObject*)pGraphObject)->SetSceneObjectActive(IsActive);
	return EPerformNodeResult::Successed;
}

FText USetSceneObjectActiveNode::GetNodeTitle() const
{
	return IsActive ? FText::FromString("Active") : FText::FromString("Unactive");
}

#if WITH_EDITORONLY_DATA

void USetSceneObjectActiveNode::GetXMLSavingProperty(std::map<FString, XMLProperty>& Properties)
{
	Super::GetXMLSavingProperty(Properties);

	Properties.insert(std::pair<FString, XMLProperty>("IsActive", XMLProperty(IsActive ? "true" : "false")));
}

void USetSceneObjectActiveNode::LoadPropertyFromXML(std::map<FString, XMLProperty>& Properties)
{
	IsActive = Properties["IsActive"].Val == "true";

	Super::LoadPropertyFromXML(Properties);
}
#endif //WITH_EDITORONLY_DATA

//UGetStoryGraphObjectNode.................................................................

UGetStoryGraphObjectStateNode::UGetStoryGraphObjectStateNode()
{
	NodeType = ENodeType::GetStoryGraphObjectState;
	WantedObjectState = 0;

	NodePins.Add(FStoryGraphPin(EEdGraphPinDirection::EGPD_Input, EPinDataTypes::PinType_Vertical));
	NodePins.Add(FStoryGraphPin(EEdGraphPinDirection::EGPD_Output, EPinDataTypes::PinType_Vertical));
}

EPerformNodeResult UGetStoryGraphObjectStateNode::PerformNode()
{
	Super::PerformNode();

	UStoryGraphInventoryItem* InventoryItem = Cast<UStoryGraphInventoryItem>(pGraphObject);

	if (bPerformNode)
	{
		return EPerformNodeResult::NodeAlreadyPerformed;
	}


	if (pGraphObject->GetCurrentState() == WantedObjectState)
	{
		bPerformNode = true;
		return EPerformNodeResult::Successed;
	}
	else
	{
		return EPerformNodeResult::Fail;
	}
}

FText UGetStoryGraphObjectStateNode::GetNodeTitle() const
{
	TArray<FString> ObjectStates;
	if (pGraphObject)
	{
		pGraphObject->GetObjectStateAsString(ObjectStates);
		if (ObjectStates.Num() > 0) return FText::FromString(ObjectStates[WantedObjectState]);
		else return FText::FromString("Non");
	}
	else
	{
		return FText::FromString("Non");
	}
}

void UGetStoryGraphObjectStateNode::SetWantedObjectState(int WantedState_)
{
	WantedObjectState = WantedState_;
#if WITH_EDITORONLY_DATA
	UpdateGraphNode();
#endif //WITH_EDITORONLY_DATA
}

#if WITH_EDITORONLY_DATA

void UGetStoryGraphObjectStateNode::GetXMLSavingProperty(std::map<FString, XMLProperty>& Properties)
{
	Super::GetXMLSavingProperty(Properties);

	Properties.insert(
		std::pair<FString, XMLProperty>("WantedObjectState", XMLProperty(FString::FromInt(WantedObjectState))));
}

void UGetStoryGraphObjectStateNode::LoadPropertyFromXML(std::map<FString, XMLProperty>& Properties)
{
	WantedObjectState = FCString::Atoi(*Properties["WantedObjectState"].Val);

	Super::LoadPropertyFromXML(Properties);
}
#endif //WITH_EDITORONLY_DATA
//UAddDialogNode...........................................................................................

UAddDialogNode::UAddDialogNode()
{
	NodeType = ENodeType::AddDialog;
	Activate = true;

	NodePins.Add(FStoryGraphPin(EEdGraphPinDirection::EGPD_Input, EPinDataTypes::PinType_Horizontal));
	NodePins.Add(FStoryGraphPin(EEdGraphPinDirection::EGPD_Output, EPinDataTypes::PinType_Horizontal));
}

EPerformNodeResult UAddDialogNode::PerformNode()
{
	Super::PerformNode();

	SelectedDialog->IsActive = Activate;
	return EPerformNodeResult::Successed;
}

FText UAddDialogNode::GetNodeTitle() const
{
	return SelectedDialog && SelectedDialog->Dialogs.Num() > 0
		       ? UDialogNodeBase::GetDialogName(SelectedDialog->Dialogs[0]->Dialog)
		       : FText::FromString("Not Defined");
}

#if WITH_EDITORONLY_DATA
void UAddDialogNode::SetCurrentDialog(UDialogStartNode* SelectedDialog_)
{
	SelectedDialog = SelectedDialog_;

	UpdateGraphNode();
}


void UAddDialogNode::RefreshColor()
{
	if (Activate)
	{
		NodeColor = UCustomNodeBase::GetColorFromNodeType(ENodeType::AddDialog, 0);
	}
	else
	{
		NodeColor = UCustomNodeBase::GetColorFromNodeType(ENodeType::AddDialog, 1);
	}
}

void UAddDialogNode::GetXMLSavingProperty(std::map<FString, XMLProperty>& Properties)
{
	Super::GetXMLSavingProperty(Properties);

	Properties.insert(std::pair<FString, XMLProperty>("SelectedDialog", XMLProperty(SelectedDialog->XMLID)));
	Properties.insert(std::pair<FString, XMLProperty>("Activate", XMLProperty(Activate ? "true" : "false")));
}

void UAddDialogNode::LoadPropertyFromXML(std::map<FString, XMLProperty>& Properties)
{
	int32 SignNum = Properties["SelectedDialog"].Val.Find("_");
	UStoryGraphObject* CurrentStoryGarphObject = pStoryGraph->GraphObjects[FCString::Atoi(
		*Properties["SelectedDialog"].Val.Left(SignNum))];
	if (UStoryGraphCharacter* pStoryGraphCharecter = Cast<UStoryGraphCharacter>(CurrentStoryGarphObject))
	{
		SelectedDialog = (UDialogStartNode*)pStoryGraphCharecter->GraphNodes[FCString::Atoi(
			*Properties["SelectedDialog"].Val.RightChop(SignNum + 1))];
	}
	else if (UStoryGraphPlaceTrigger* pStoryGraphPlaceTrigger = Cast<UStoryGraphPlaceTrigger>(CurrentStoryGarphObject))
	{
		SelectedDialog = (UDialogStartNode*)pStoryGraphPlaceTrigger->GraphNodes[FCString::Atoi(
			*Properties["SelectedDialog"].Val.RightChop(SignNum + 1))];
	}

	Activate = Properties["Activate"].Val == "true";

	Super::LoadPropertyFromXML(Properties);
}
#endif //WITH_EDITORONLY_DATA
//UAddDialogFromDialogNode...........................................................................................

UAddDialogFromDialogNode::UAddDialogFromDialogNode()
{
	NodeType = ENodeType::AddDialogFromDialog;
	Activate = true;

	NodePins.Add(FStoryGraphPin(EEdGraphPinDirection::EGPD_Input, EPinDataTypes::PinType_Horizontal));
	NodePins.Add(FStoryGraphPin(EEdGraphPinDirection::EGPD_Output, EPinDataTypes::PinType_Horizontal));
}

EPerformNodeResult UAddDialogFromDialogNode::PerformNode()
{
	Super::PerformNode();

	SelectedDialog->IsActive = Activate;

	return EPerformNodeResult::Successed;
}

FText UAddDialogFromDialogNode::GetNodeTitle() const
{
	return SelectedDialog && SelectedDialog->Dialogs.Num() > 0
		       ? UDialogNodeBase::GetDialogName(SelectedDialog->Dialogs[0]->Dialog)
		       : FText::FromString("Not Defind");
}

void UAddDialogFromDialogNode::SetCurrentDialog(UDialogStartNode* SelectedDialog_)
{
	SelectedDialog = SelectedDialog_;
#if WITH_EDITORONLY_DATA
	UpdateGraphNode();
#endif //WITH_EDITORONLY_DATA
}

#if WITH_EDITORONLY_DATA

void UAddDialogFromDialogNode::GetXMLSavingProperty(std::map<FString, XMLProperty>& Properties)
{
	Super::GetXMLSavingProperty(Properties);

	Properties.insert(std::pair<FString, XMLProperty>("SelectedDialog", XMLProperty(SelectedDialog->XMLID)));
	Properties.insert(std::pair<FString, XMLProperty>("Activate", XMLProperty(Activate ? "true" : "false")));
}

void UAddDialogFromDialogNode::LoadPropertyFromXML(std::map<FString, XMLProperty>& Properties)
{
	int32 SignNum = Properties["SelectedDialog"].Val.Find("_");
	UStoryGraphObject* CurrentStoryGarphObject = pStoryGraph->GraphObjects[FCString::Atoi(
		*Properties["SelectedDialog"].Val.Left(SignNum))];
	if (UStoryGraphCharacter* pStoryGraphCharecter = Cast<UStoryGraphCharacter>(CurrentStoryGarphObject))
	{
		SelectedDialog = (UDialogStartNode*)pStoryGraphCharecter->GraphNodes[FCString::Atoi(
			*Properties["SelectedDialog"].Val.RightChop(SignNum + 1))];
	}
	else if (UStoryGraphPlaceTrigger* pStoryGraphPlaceTrigger = Cast<UStoryGraphPlaceTrigger>(CurrentStoryGarphObject))
	{
		SelectedDialog = (UDialogStartNode*)pStoryGraphPlaceTrigger->GraphNodes[FCString::Atoi(
			*Properties["SelectedDialog"].Val.RightChop(SignNum + 1))];
	}

	Activate = Properties["Activate"].Val == "true";

	Super::LoadPropertyFromXML(Properties);
}

#endif //WITH_EDITORONLY_DATA

//UCancelQuestNode.............................................................................................

UCancelQuestNode::UCancelQuestNode()
{
	NodeType = ENodeType::CancelQuest;

	NodePins.Add(FStoryGraphPin(EEdGraphPinDirection::EGPD_Input, EPinDataTypes::PinType_Horizontal));
	NodePins.Add(FStoryGraphPin(EEdGraphPinDirection::EGPD_Output, EPinDataTypes::PinType_Horizontal));
}

EPerformNodeResult UCancelQuestNode::PerformNode()
{
	Super::PerformNode();
	((UStoryGraphQuest*)pGraphObject)->SetCurrentState((int)EQuestStates::Canceled);

	return EPerformNodeResult::Successed;
}

//UQuestStartNode.......................................................................................................

UQuestStartNode::UQuestStartNode()
{
	NodeType = ENodeType::QuestStart;

	NodePins.Add(FStoryGraphPin(EEdGraphPinDirection::EGPD_Output, EPinDataTypes::PinType_Vertical));
	NodePins.Add(FStoryGraphPin(EEdGraphPinDirection::EGPD_Output, EPinDataTypes::PinType_Horizontal));
}


FText UQuestStartNode::GetNodeTitle() const
{
	return FText::FromString("Start");
}
#if WITH_EDITORONLY_DATA
void UQuestStartNode::InitNode(class UStoryGraphObject* pGraphObject_)
{
	Super::InitNode(pGraphObject_);

	pQuestOwner = (UStoryGraphQuest*)pGraphObject_;
}

void UQuestStartNode::RefreshColor()
{
	UStoryGraphQuest* Quest = Cast<UStoryGraphQuest>(pGraphObject);
	if (Quest)
	{
		NodeColor = Quest->MainQuest
			            ? UCustomNodeBase::GetColorFromNodeType(ENodeType::QuestStart, 0)
			            : NodeColor = UCustomNodeBase::GetColorFromNodeType(ENodeType::QuestStart, 1);
		UpdateGraphNode();
	}
}

void UQuestStartNode::RefreshQuestOwner()
{
	pQuestOwner = (UStoryGraphQuest*)pGraphObject;

	RefreshColor();
	UpdateGraphNode();

	TArray<UCustomNodeBase*> ChildNodes;

	GetChildNodes(ChildNodes, EPinDataTypes::PinType_Vertical);

	for (int i = 0; i < ChildNodes.Num(); i++)
	{
		((UStoryVerticalNodeBase*)ChildNodes[i])->RefreshQuestOwner();
	}
}
#endif //WITH_EDITORONLY_DATA
//UEndGameNode..................................................................................................................

UEndGameNode::UEndGameNode()
{
	NodeType = ENodeType::EndGame;

	NodePins.Add(FStoryGraphPin(EEdGraphPinDirection::EGPD_Input, EPinDataTypes::PinType_Vertical));
}

EPerformNodeResult UEndGameNode::PerformNode()
{
	Super::PerformNode();

	if (pStoryGraph && pStoryGraph->OwnedActor)
	{
		if (AHUD_StoryGraph* HUD = Cast<AHUD_StoryGraph>(
			pStoryGraph->OwnedActor->GetWorld()->GetFirstPlayerController()->GetHUD()))
		{
			HUD->EndGame(pQuestPhase->Description);
		}
		else
		{
			UE_LOG(LogCategoryStoryGraphPluginRuntime, Warning, TEXT("Your HUD should inherit AHUD_StoryGraph class"));
		}
	}

	return EPerformNodeResult::Successed;
}

#if WITH_EDITORONLY_DATA
void UEndGameNode::RefreshColor()
{
	if (pQuestOwner)
	{
		if (pQuestOwner->MainQuest)
		{
			NodeColor = UCustomNodeBase::GetColorFromNodeType(ENodeType::AddQuestPhase, 1);
		}
		else
		{
			NodeColor = UCustomNodeBase::GetColorFromNodeType(ENodeType::AddQuestPhase, 2);
		}
	}
	else
	{
		NodeColor = UCustomNodeBase::GetColorFromNodeType(ENodeType::AddQuestPhase, 0);
	}
}
#endif //WITH_EDITORONLY_DATA

//USendMessageNode..................................................................................................................
USendMessageNode::USendMessageNode()
{
	NodeType = ENodeType::SendMessageScene;

	NodePins.Add(FStoryGraphPin(EEdGraphPinDirection::EGPD_Input, EPinDataTypes::PinType_Horizontal));
	NodePins.Add(FStoryGraphPin(EEdGraphPinDirection::EGPD_Output, EPinDataTypes::PinType_Horizontal));
}

EPerformNodeResult USendMessageNode::PerformNode()
{
	Super::PerformNode();

	((UStoryGraphObjectWithSceneObject*)pGraphObject)->SendMessageToSceneObject(Message);
	return EPerformNodeResult::Successed;
}

FText USendMessageNode::GetNodeTitle() const
{
	return FText::FromString("Msg: " + Message);
}

#if WITH_EDITORONLY_DATA

void USendMessageNode::GetXMLSavingProperty(std::map<FString, XMLProperty>& Properties)
{
	Super::GetXMLSavingProperty(Properties);

	Properties.insert(std::pair<FString, XMLProperty>("Message", XMLProperty(Message)));
}


void USendMessageNode::LoadPropertyFromXML(std::map<FString, XMLProperty>& Properties)
{
	Message = Properties["Message"].Val;

	Super::LoadPropertyFromXML(Properties);
}

#endif //WITH_EDITORONLY_DATA

//UPrintStringNode..................................................................................................................
UPrintStringNode::UPrintStringNode()
{
	NodeType = ENodeType::PrintString;
	InString = "Hello";
	PrintToScreen = true;
	PrintToLog = true;
	Duration = 2.0;
	TextColor = FColor::Blue;

	NodePins.Add(FStoryGraphPin(EEdGraphPinDirection::EGPD_Input, EPinDataTypes::PinType_Horizontal));
	NodePins.Add(FStoryGraphPin(EEdGraphPinDirection::EGPD_Output, EPinDataTypes::PinType_Horizontal));
	NodePins.Add(FStoryGraphPin(EEdGraphPinDirection::EGPD_Input, EPinDataTypes::PinType_Vertical));
	NodePins.Add(FStoryGraphPin(EEdGraphPinDirection::EGPD_Output, EPinDataTypes::PinType_Vertical));
}


EPerformNodeResult UPrintStringNode::PerformNode()
{
	Super::PerformNode();

	if (bPerformNode)
	{
		return EPerformNodeResult::NodeAlreadyPerformed;
	}
	else
	{
		if (PrintToScreen) GEngine->AddOnScreenDebugMessage(-1, Duration, TextColor, InString);
		if (PrintToLog) UE_LOG(LogCategoryStoryGraphPluginRuntime, Warning, TEXT("%s"), *InString);
		bPerformNode = true;
		return EPerformNodeResult::Successed;
	}
}

FText UPrintStringNode::GetNodeTitle() const
{
	return FText::FromString(InString);
}

#if WITH_EDITORONLY_DATA

void UPrintStringNode::GetXMLSavingProperty(std::map<FString, XMLProperty>& Properties)
{
	Super::GetXMLSavingProperty(Properties);

	Properties.insert(std::pair<FString, XMLProperty>("InString", XMLProperty(InString)));
	Properties.insert(std::pair<FString, XMLProperty>("PrintToScreen", XMLProperty(PrintToScreen ? "true" : "false")));
	Properties.insert(std::pair<FString, XMLProperty>("PrintToLog", XMLProperty(PrintToLog ? "true" : "false")));
	Properties.insert(std::pair<FString, XMLProperty>("Duration", XMLProperty(FString::SanitizeFloat(Duration))));
}

void UPrintStringNode::LoadPropertyFromXML(std::map<FString, XMLProperty>& Properties)
{
	InString = Properties["InString"].Val;
	PrintToScreen = Properties["PrintToScreen"].Val == "true";
	PrintToLog = Properties["PrintToLog"].Val == "true";
	Duration = FCString::Atof(*Properties["Duration"].Val);

	Super::LoadPropertyFromXML(Properties);
}

#endif //WITH_EDITORONLY_DATA

//UAddScreenMessageNode..................................................................................................................
UAddScreenMessageNode::UAddScreenMessageNode()
{
	NodeType = ENodeType::AddScreenMessage;
	Message = FText::FromString("Hello");
	Duration = 5.0;

	NodePins.Add(FStoryGraphPin(EEdGraphPinDirection::EGPD_Input, EPinDataTypes::PinType_Horizontal));
	NodePins.Add(FStoryGraphPin(EEdGraphPinDirection::EGPD_Output, EPinDataTypes::PinType_Horizontal));
}

EPerformNodeResult UAddScreenMessageNode::PerformNode()
{
	Super::PerformNode();
	if (pStoryGraph && pStoryGraph->OwnedActor)
	{
		if (AHUD_StoryGraph* HUD = Cast<AHUD_StoryGraph>(
			pStoryGraph->OwnedActor->GetWorld()->GetFirstPlayerController()->GetHUD()))
		{
			if (HUD->GameScreen)
			{
				HUD->GameScreen->AddMessageOnScreen(Message, Duration);
			}
		}
	}

	return EPerformNodeResult::Successed;
}

FText UAddScreenMessageNode::GetNodeTitle() const
{
	return Message;
}

#if WITH_EDITORONLY_DATA

void UAddScreenMessageNode::GetXMLSavingProperty(std::map<FString, XMLProperty>& Properties)
{
	Super::GetXMLSavingProperty(Properties);

	Properties.insert(std::pair<FString, XMLProperty>("Message", XMLProperty(Message.ToString())));
	Properties.insert(std::pair<FString, XMLProperty>("Duration", XMLProperty(FString::SanitizeFloat(Duration))));
}

void UAddScreenMessageNode::LoadPropertyFromXML(std::map<FString, XMLProperty>& Properties)
{
	Message = FText::FromString(Properties["Message"].Val);
	Duration = FCString::Atof(*Properties["Duration"].Val);

	Super::LoadPropertyFromXML(Properties);
}
#endif //WITH_EDITORONLY_DATA

//UAddTargetObjectToPhaseNode..................................................................................................................
UAddTargetObjectToPhaseNode::UAddTargetObjectToPhaseNode()
{
	NodeType = ENodeType::AddTargetObjectToPhase;

	NodePins.Add(FStoryGraphPin(EEdGraphPinDirection::EGPD_Input, EPinDataTypes::PinType_Horizontal));
	NodePins.Add(FStoryGraphPin(EEdGraphPinDirection::EGPD_Output, EPinDataTypes::PinType_Horizontal));
}

EPerformNodeResult UAddTargetObjectToPhaseNode::PerformNode()
{
	Super::PerformNode();

	if (pQuestPhase)
	{
		TArray<AActor*> ScenObjects;
		((UStoryGraphObjectWithSceneObject*)pGraphObject)->GetSceneObjects(ScenObjects);
		pQuestPhase->PhaseObjects.Append(ScenObjects);
	}

	return EPerformNodeResult::Successed;
}

FText UAddTargetObjectToPhaseNode::GetNodeTitle() const
{
	return FText::FromString("Radar");
}

//UMessageStartNode..................................................................................................

UMessageStartNode::UMessageStartNode()
{
	NodeType = ENodeType::MessageStart;
}

//UMessageNode...........................................................................................................

UMessageNode::UMessageNode()
{
	NodeType = ENodeType::Message;
}

//UMessageEndNode.....................................................................................................

UMessageEndNode::UMessageEndNode()
{
	NodeType = ENodeType::MessageEnd;
}

//UMessageExitNode........................................................................................................

UMessageExitNode::UMessageExitNode()
{
	NodeType = ENodeType::MessageExit;
}

//UActivateTriggerNode..................................................................................................................
UActivateTriggerNode::UActivateTriggerNode()
{
	NodeType = ENodeType::ActivateTrigger;

	NodePins.Add(FStoryGraphPin(EEdGraphPinDirection::EGPD_Input, EPinDataTypes::PinType_Horizontal));
	NodePins.Add(FStoryGraphPin(EEdGraphPinDirection::EGPD_Output, EPinDataTypes::PinType_Horizontal));
}

EPerformNodeResult UActivateTriggerNode::PerformNode()
{
	Super::PerformNode();
	((UStoryGraphPlaceTrigger*)GetOuter())->SetCurrentState((int)EPlaceTriggerStates::Active);
	return EPerformNodeResult::Successed;
}

FText UActivateTriggerNode::GetNodeTitle() const
{
	return FText::FromString("Activate");
}


//USetInventoryItemStateNode..................................................................................................................
USetInventoryItemStateNode::USetInventoryItemStateNode()
{
	NodeType = ENodeType::SetInventoryItemState;

	NodePins.Add(FStoryGraphPin(EEdGraphPinDirection::EGPD_Input, EPinDataTypes::PinType_Horizontal));
	NodePins.Add(FStoryGraphPin(EEdGraphPinDirection::EGPD_Output, EPinDataTypes::PinType_Horizontal));
}

EPerformNodeResult USetInventoryItemStateNode::PerformNode()
{
	Super::PerformNode();

	pGraphObject->SetCurrentState(NewCurrentInventoryItemState);

	return EPerformNodeResult::Successed;
}

FText USetInventoryItemStateNode::GetNodeTitle() const
{
	if (pGraphObject)
	{
		TArray<FString> ObjectStates;
		pGraphObject->GetObjectStateAsString(ObjectStates);
		return FText::FromString(ObjectStates[NewCurrentInventoryItemState]);
	}
	else
	{
		return FText::FromString("Non");
	}
}

void USetInventoryItemStateNode::SetCurrentState(int State)
{
	NewCurrentInventoryItemState = State;
#if WITH_EDITORONLY_DATA
	UpdateGraphNode();
#endif //WITH_EDITORONLY_DATA
}

#if WITH_EDITORONLY_DATA

void USetInventoryItemStateNode::GetXMLSavingProperty(std::map<FString, XMLProperty>& Properties)
{
	Super::GetXMLSavingProperty(Properties);

	Properties.insert(std::pair<FString, XMLProperty>("NewCurrentInventoryItemState",
	                                                  XMLProperty(FString::FromInt(NewCurrentInventoryItemState))));
}

void USetInventoryItemStateNode::LoadPropertyFromXML(std::map<FString, XMLProperty>& Properties)
{
	NewCurrentInventoryItemState = FCString::Atoi(*Properties["NewCurrentInventoryItemState"].Val);

	Super::LoadPropertyFromXML(Properties);
}

#endif //WITH_EDITORONLY_DATA

//USetInventoryItemStateFromMessageNode......................................................

USetInventoryItemStateFromMessageNode::USetInventoryItemStateFromMessageNode()
{
	NodeType = ENodeType::SetInventoryItemStateFromMessage;
}

//UPrintQuestPhaseOnScreenNode..................................................................................................................
UPrintQuestPhaseOnScreenNode::UPrintQuestPhaseOnScreenNode()
{
	NodeType = ENodeType::PrintQuestPhaseOnScreen;
	NodePins.Add(FStoryGraphPin(EEdGraphPinDirection::EGPD_Input, EPinDataTypes::PinType_Horizontal));
	NodePins.Add(FStoryGraphPin(EEdGraphPinDirection::EGPD_Output, EPinDataTypes::PinType_Horizontal));
}

EPerformNodeResult UPrintQuestPhaseOnScreenNode::PerformNode()
{
	Super::PerformNode();

	if (pStoryGraph && pStoryGraph->OwnedActor)
	{
		AHUD_StoryGraph* HUD = Cast<AHUD_StoryGraph>(
			pStoryGraph->OwnedActor->GetWorld()->GetFirstPlayerController()->GetHUD());
		if (HUD)
		{
			HUD->PrintQuestPhaseOnScreen(pQuestPhase->Description);
		}
		else
		{
			UE_LOG(LogCategoryStoryGraphPluginRuntime, Warning, TEXT("Your HUD should inherit AHUD_StoryGraph class"));
		}
	}
	return EPerformNodeResult::Successed;
}

FText UPrintQuestPhaseOnScreenNode::GetNodeTitle() const
{
	return FText::FromString("PrintOnScreen");
}

//UAddMessageBranchNode..................................................................................................................
UAddMessageBranchNode::UAddMessageBranchNode()
{
	NodeType = ENodeType::AddMessageBranch;
}


//USendMessageToLevelBlueprintNode..................................................................................................................
USendMessageToLevelBlueprintNode::USendMessageToLevelBlueprintNode()
{
	NodeType = ENodeType::SendMessageToLevelBlueprint;

	NodePins.Add(FStoryGraphPin(EEdGraphPinDirection::EGPD_Input, EPinDataTypes::PinType_Horizontal));
	NodePins.Add(FStoryGraphPin(EEdGraphPinDirection::EGPD_Output, EPinDataTypes::PinType_Horizontal));
}

EPerformNodeResult USendMessageToLevelBlueprintNode::PerformNode()
{
	Super::PerformNode();

	if (pStoryGraph && pStoryGraph->OwnedActor)
	{
		ALevelScriptActor_StoryGraph* LevelBlueprint = Cast<ALevelScriptActor_StoryGraph>(
			pStoryGraph->OwnedActor->GetWorld()->GetLevelScriptActor());

		if (LevelBlueprint)
		{
			LevelBlueprint->GetMessageFromStoryGraph(Message);
		}
		else
		{
			UE_LOG(LogCategoryStoryGraphPluginRuntime, Warning, TEXT("Reparen level blueprint"));
		}
	}

	return EPerformNodeResult::Successed;
}

FText USendMessageToLevelBlueprintNode::GetNodeTitle() const
{
	return FText::FromString("Msg: " + Message);
}


#if WITH_EDITORONLY_DATA

void USendMessageToLevelBlueprintNode::GetXMLSavingProperty(std::map<FString, XMLProperty>& Properties)
{
	Super::GetXMLSavingProperty(Properties);

	Properties.insert(std::pair<FString, XMLProperty>("Message", XMLProperty(Message)));
}

void USendMessageToLevelBlueprintNode::LoadPropertyFromXML(std::map<FString, XMLProperty>& Properties)
{
	Message = Properties["Message"].Val;

	Super::LoadPropertyFromXML(Properties);
}

#endif //WITH_EDITORONLY_DATA
