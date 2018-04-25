// Copyright 2016 Dmitriy Pavlov

#include "GraphNodes_StoryGraph.h"
#include "ScopedTransaction.h"
#include "EditorStyleSet.h"
#include "ScopedTransaction.h"
#include "Runtime/Engine/Classes/EdGraph/EdGraph.h"
#include "GraphEditor.h"
#include "SInlineEditableTextBlock.h"
#include "AssetThumbnail.h"
#include "AssetRegistryModule.h"
#include "SNumericEntryBox.h"
#include "ProxyNods.h"
#include "NodeStyle.h"

//PinNode..........................................................................
void SCustomOutputPin::Construct(const FArguments& InArgs, UEdGraphPin* InPin, int PinNumber)
{
	this->SetCursor(EMouseCursor::Default);

	typedef SCustomOutputPin ThisClass;

	bShowLabel = true;
	IsEditable = true;

	GraphPinObj = InPin;
	check(GraphPinObj != NULL);

	TSharedPtr<SOverlay> PinContent = SNew(SOverlay);

	if (PinNumber)
	{
		IsPinNumbered = true;
		PinContent->AddSlot()
			.HAlign(HAlign_Center)
			.VAlign(EVerticalAlignment::VAlign_Fill)
			[
				SNew(STextBlock)
				.Text(FText::FromString(FString::FromInt(PinNumber)))
				.Font(FSlateFontInfo(FPaths::EngineContentDir() / TEXT("Slate/Fonts/Roboto.ttf"), 7))
			];
	}
	const UEdGraphSchema* Schema = GraphPinObj->GetSchema();
	check(Schema);

	
	
	SBorder::Construct(SBorder::FArguments()
		.BorderImage(this, &SCustomOutputPin::GetPinBorder)
		.BorderBackgroundColor(this, &ThisClass::GetPinColor)
		.OnMouseButtonDown(this, &ThisClass::OnPinMouseDown)
		.Cursor(this, &ThisClass::GetPinCursor)
		.Padding(this, &ThisClass::GetPadding)
		[
			PinContent.ToSharedRef()
		]
		);
}

TSharedRef<SWidget>	SCustomOutputPin::GetDefaultValueWidget()
{
	return SNew(STextBlock);
}

const FSlateBrush* SCustomOutputPin::GetPinBorder() const
{
	return FEditorStyle::GetBrush(TEXT("Graph.StateNode.Body"));
}

FSlateColor SCustomOutputPin::GetPinColor() const
{
	static const FLinearColor MeshPinColor(1.0f, 0.7f, 0.0f);
	static const FLinearColor MarkerPinColor(0.3f, 0.3f, 1.0f);
	static const FLinearColor DarkColor(0.02f, 0.02f, 0.02f);
	if (!IsHovered()) {
		return DarkColor;
	}
	
	return MeshPinColor;
}

FMargin SCustomOutputPin::GetPadding() const
{
	if (IsPinNumbered)
	{
		return FMargin(0.0f);
	}
	return FMargin(5.0f);
}

//CustomNodeBase............................................................................................................


void  SGraphNode_CustomNodeBase::Construct(const FArguments& InArgs, UProxyNodeBase* InNode)
{
	GraphNode = InNode;
	NodeBace = InNode;
	this->SetCursor(EMouseCursor::CardinalCross);
	this->UpdateGraphNode();
	
	
		
	SWidget::SetToolTip(CreateToolTipWiget());
}

TSharedPtr<IToolTip> SGraphNode_CustomNodeBase::CreateToolTipWiget()
{
	return SNew(SToolTip)
		.BorderImage(FCoreStyle::Get().GetBrush("ToolTip.BrightBackground"))
		.Text(GetNodeTooltip());
}

void SGraphNode_CustomNodeBase::UpdateGraphNode()
{
	
	InputPins.Empty();
	OutputPins.Empty();

	// Reset variables that are going to be exposed, in case we are refreshing an already setup node.
	RightNodeBox.Reset();
	LeftNodeBox.Reset();
	TopNodeBox.Reset();
	BottomNodeBox.Reset();

	//Create Border
	this->ContentScale.Bind(this, &SGraphNode::GetContentScale);
	this->GetOrAddSlot(ENodeZone::Center)
		.HAlign(HAlign_Center)
		.VAlign(VAlign_Center)
		[
			SNew(SBorder)
			.BorderImage(FEditorStyle::GetBrush("Graph.StateNode.Body"))
			.Padding(0.0f)
			.BorderBackgroundColor(NodeBace->CustomNode->NodeColor)
			[
				SNew(SOverlay)

				//LEFT INPUT PIN AREA
				+ SOverlay::Slot()
				.HAlign(HAlign_Left)
				.VAlign(VAlign_Fill)
				.Padding(0.0f, 10.0f)
				
				[
					SAssignNew(LeftNodeBox, SVerticalBox)
				]

				//RIGHT OUTPUT PIN AREA
				+ SOverlay::Slot()
					.HAlign(HAlign_Right)
					.VAlign(VAlign_Fill)
					.Padding(0.0f,10.0f)
					[
						SAssignNew(RightNodeBox, SVerticalBox)
					]

				//TOP INPUT PIN AREA				
				+ SOverlay::Slot()
				.HAlign(HAlign_Fill)
				.VAlign(VAlign_Top)
				[
					SAssignNew(TopNodeBox, SHorizontalBox)
				]

				//BOTTOM OUTPUT PIN AREA				
				+ SOverlay::Slot()
					.HAlign(HAlign_Fill)
					.VAlign(VAlign_Bottom)
					[
						SAssignNew(BottomNodeBox, SHorizontalBox)
					]

				//NodeWidget
				+ SOverlay::Slot()
				.HAlign(HAlign_Center)
				.VAlign(VAlign_Center)
				.Padding(10.0f)
				[
					SAssignNew(NodeWiget, SOverlay)
				]
				
			]
		];

	CreateNodeWidget();
	CreatePinWidgets();
}

void SGraphNode_CustomNodeBase::CreateNodeWidget()
{
	FLinearColor TitleShadowColor(0.6f, 0.6f, 0.6f);
	TSharedPtr<SNodeTitle> NodeTitle = SNew(SNodeTitle, GraphNode);
	TSharedPtr<SVerticalBox> NodeBox;

	NodeWiget->AddSlot()
	[
		SNew(SBorder)
		.BorderImage(FEditorStyle::GetBrush("Graph.StateNode.ColorSpill"))
		.BorderBackgroundColor(TitleShadowColor)
		.HAlign(HAlign_Center)
		.VAlign(VAlign_Center)
		.Visibility(EVisibility::SelfHitTestInvisible)
		[
			SAssignNew(NodeBox, SVerticalBox)
		]
	];
			

	AddNodeStrings(NodeBox);
	
}

void SGraphNode_CustomNodeBase::AddNodeStrings(TSharedPtr<SVerticalBox> NodeBox)
{
	const FSlateBrush* NodeTypeIcon = GetNameIcon();
	if (UCustomNodeBase::GetIconNameFromNodeType(NodeBace->CustomNode->NodeType) == FName("Non"))
	{
		NodeBox->AddSlot()
			.AutoHeight()
			[
				SNew(SHorizontalBox)

				+ SHorizontalBox::Slot()
			.AutoWidth()
			.VAlign(VAlign_Center)
			[
				SNew(SImage)
				.Image(NodeTypeIcon)
			]
		+ SHorizontalBox::Slot()
			.Padding(FMargin(4.0f, 0.0f, 4.0f, 0.0f))
			[
				SNew(STextBlock)
				.Text(NodeBace->GetNodeTitle(ENodeTitleType::FullTitle))
			.Font(FSlateFontInfo(FPaths::EngineContentDir() / TEXT("Slate/Fonts/Roboto-Bold.ttf"), 16))
			]
			];
	}
	else
	{
		NodeBox->AddSlot()
			.HAlign(HAlign_Center)
			.AutoHeight()
			[
				SNew(SImage)
				.Image(FNodeStyle::Get().GetBrush(UCustomNodeBase::GetIconNameFromNodeType(NodeBace->CustomNode->NodeType)))
			];
	}
	
}

void SGraphNode_CustomNodeBase::CreatePinWidgets()
{
	
	for (int i = 0; i < NodeBace->Pins.Num(); i++)
	{
		TSharedPtr<SGraphPin> NewPin = SNew(SCustomOutputPin, NodeBace->Pins[i]);
		NewPin->SetIsEditable(IsEditable);
		AddPin(NewPin.ToSharedRef());

		if (NodeBace->Pins[i]->Direction == EEdGraphPinDirection::EGPD_Input)
		{
			InputPins.Add(NewPin.ToSharedRef());
		}
		else
		{
			OutputPins.Add(NewPin.ToSharedRef());
		}
	}

}

void SGraphNode_CustomNodeBase::AddPin(const TSharedRef<SGraphPin>& PinToAdd)
{
	PinToAdd->SetOwner(SharedThis(this));

	const UEdGraphPin* PinObj = PinToAdd->GetPinObj();
	const bool bAdvancedParameter = PinObj && PinObj->bAdvancedView;
	if (bAdvancedParameter)
	{
		PinToAdd->SetVisibility(TAttribute<EVisibility>(PinToAdd, &SGraphPin::IsPinVisibleAsAdvanced));
	}

	if (PinToAdd->GetDirection() == EEdGraphPinDirection::EGPD_Input)
	{
		if (PinToAdd->GetPinObj()->PinType.PinCategory.ToString() == UCustomNodeBase::GetPinDataTypeEnumAsString(EPinDataTypes::PinType_Vertical))
		{
			TopNodeBox->AddSlot()
				.HAlign(HAlign_Fill)
				.VAlign(VAlign_Fill)
				.FillWidth(1.0f)
				//.Padding(20.0f, 0.0f)
				[
					PinToAdd
				];
			InputPins.Add(PinToAdd);
		}
		else if (PinToAdd->GetPinObj()->PinType.PinCategory.ToString() == UCustomNodeBase::GetPinDataTypeEnumAsString(EPinDataTypes::PinType_Horizontal))
		{
			LeftNodeBox->AddSlot()
				.HAlign(HAlign_Fill)
				.VAlign(VAlign_Fill)
				.FillHeight(1.0f)
				//.Padding(20.0f, 0.0f)
				[
					PinToAdd
				];
			InputPins.Add(PinToAdd);
		}
	}
	
	
	else  // Direction == EEdGraphPinDirection::EGPD_Output
	{
		if (PinToAdd->GetPinObj()->PinType.PinCategory.ToString() == UCustomNodeBase::GetPinDataTypeEnumAsString(EPinDataTypes::PinType_Vertical))
		{
			BottomNodeBox->AddSlot()
				.HAlign(HAlign_Fill)
				.VAlign(VAlign_Fill)
				.FillWidth(1.0f)
				[
					PinToAdd
				];
			OutputPins.Add(PinToAdd);
		}
		else if (PinToAdd->GetPinObj()->PinType.PinCategory.ToString() == UCustomNodeBase::GetPinDataTypeEnumAsString(EPinDataTypes::PinType_Horizontal))
		{
			RightNodeBox->AddSlot()
				.HAlign(HAlign_Fill)
				.VAlign(VAlign_Fill)
				.FillHeight(1.0f)
				//.Padding(20.0f, 0.0f)
				[
					PinToAdd
				];
			InputPins.Add(PinToAdd);
		}
	}
}


const FSlateBrush* SGraphNode_CustomNodeBase::GetNameIcon() const
{
	return FEditorStyle::GetBrush(TEXT("Graph.StateNode.Icon"));
}


//StoryGraphDependetNode.......................................................................................................

void SGraphNode_StoryGraphDependetNode::AddNodeStrings(TSharedPtr<SVerticalBox> NodeBox)
{
	if (NodeBace->CustomNode->pGraphObject)
	{
		NodeBox->AddSlot()
			//.VAlign(VAlign_Center)
			.HAlign(HAlign_Center)
			.AutoHeight()
			[
				SNew(STextBlock)
				.Text(NodeBace->CustomNode->pGraphObject->ObjName)
				.Font(FSlateFontInfo(FPaths::EngineContentDir() / TEXT("Slate/Fonts/Roboto-Bold.ttf"), 12))
			];

		SGraphNode_CustomNodeBase::AddNodeStrings(NodeBox);
	}
}



//DialogBaceNode.......................................................................................................

void SGraphNode_DialogBase::UpdateGraphNode()
{
	
	InputPins.Empty();
	OutputPins.Empty();

	// Reset variables that are going to be exposed, in case we are refreshing an already setup node.
	RightNodeBox.Reset();
	LeftNodeBox.Reset();
	TopNodeBox.Reset();
	BottomNodeBox.Reset();

	//Create Border
	this->ContentScale.Bind(this, &SGraphNode::GetContentScale);
	this->GetOrAddSlot(ENodeZone::Center)
		.HAlign(HAlign_Center)
		.VAlign(VAlign_Center)
		[
			SNew(SBorder)
			.BorderImage(FEditorStyle::GetBrush("Graph.StateNode.Body"))
			.Padding(0.0f)
			.BorderBackgroundColor(NodeBace->CustomNode->NodeColor)
			[
				SNew(SOverlay)

				//LEFT INPUT PIN AREA
				+ SOverlay::Slot()
				.HAlign(HAlign_Left)
				.VAlign(VAlign_Fill)
				.Padding(0.0f, 10.0f)
				
				[
					SAssignNew(LeftNodeBox, SVerticalBox)
				]

				//RIGHT OUTPUT PIN AREA
				+ SOverlay::Slot()
					.HAlign(HAlign_Right)
					.VAlign(VAlign_Fill)
					.Padding(0.0f,10.0f)
					[
						SAssignNew(RightNodeBox, SVerticalBox)
					]

				//TOP INPUT PIN AREA				
				+ SOverlay::Slot()
				.HAlign(HAlign_Fill)
				.VAlign(VAlign_Top)
				[
					SAssignNew(TopNodeBox, SHorizontalBox)
				]

				//BOTTOM OUTPUT PIN AREA				
				+ SOverlay::Slot()
					.HAlign(HAlign_Fill)
					.VAlign(VAlign_Bottom)
					[
						SAssignNew(BottomNodeBox, SHorizontalBox)
					]

				//NodeWidget
				+ SOverlay::Slot()
				.HAlign(HAlign_Center)
				.VAlign(VAlign_Center)
				.Padding(17.0f,0.0f)
				[
					SAssignNew(NodeWiget, SOverlay)
				]
				
			]
		];

	CreateNodeWidget();
	CreatePinWidgets();
	
}

void SGraphNode_DialogBase::CreatePinWidgets()
{

	for (int i = 0; i < NodeBace->Pins.Num(); i++)
	{
		TSharedPtr<SGraphPin> NewPin = SNew(SGraphPin, NodeBace->Pins[i]);
		NewPin->SetIsEditable(IsEditable);
		AddPin(NewPin.ToSharedRef());

		if (NodeBace->Pins[i]->Direction == EEdGraphPinDirection::EGPD_Input)
		{
			InputPins.Add(NewPin.ToSharedRef());
		}
		else
		{
			OutputPins.Add(NewPin.ToSharedRef());
		}
	}

}

void SGraphNode_DialogBase::AddNodeStrings(TSharedPtr<SVerticalBox> NodeBox)
{
	const FSlateBrush* NodeTypeIcon = GetNameIcon();
	NodeBox->AddSlot()
		.AutoHeight()
		.Padding(0, 5)
		[
			SNew(SHorizontalBox)

			+ SHorizontalBox::Slot()
			.AutoWidth()
			.VAlign(VAlign_Center)
			[
				SNew(SImage)
				.Image(NodeTypeIcon)
			]
			+ SHorizontalBox::Slot()
			.Padding(FMargin(4.0f, 0.0f, 4.0f, 0.0f))
			[
				SNew(STextBlock)
				.Text(NodeBace->GetNodeTitle(ENodeTitleType::FullTitle))
				.Font(FSlateFontInfo(FPaths::EngineContentDir() / TEXT("Slate/Fonts/Roboto-Bold.ttf"), 16))
			]
		];
}



//DialogDependetNode.......................................................................................................

void SGraphNode_DialogDependetNode::AddNodeStrings(TSharedPtr<SVerticalBox> NodeBox)
{
	if (NodeBace->CustomNode->pGraphObject)
	{
		NodeBox->AddSlot()
			.AutoHeight()
			.HAlign(HAlign_Center)
			.Padding(0, 5)
			[
				SNew(STextBlock)
				.Text(NodeBace->CustomNode->pGraphObject->ObjName)
				.Font(FSlateFontInfo(FPaths::EngineContentDir() / TEXT("Slate/Fonts/Roboto-Bold.ttf"), 12))
			];

		SGraphNode_DialogBase::AddNodeStrings(NodeBox);
	}
}

//DialogStartNode.......................................................................................................


void SGraphNode_DialogRoot::AddNodeStrings(TSharedPtr <SVerticalBox> VerticalBox)
{
	UDialogStartNode* DialogStartNode = Cast<UDialogStartNode>(NodeBace->CustomNode);

	VerticalBox->AddSlot()
		.AutoHeight()
		.Padding(0, 5)
		[
			SNew(STextBlock)
			.Text(UDialogNodeBase::GetDialogName(DialogStartNode->Dialogs[0]->Dialog))
			.Font(FSlateFontInfo(FPaths::EngineContentDir() / TEXT("Slate/Fonts/Roboto-Bold.ttf"), 16))

		];
}

//DialogNode..................................................................................................................

void SGraphNode_Dialog::AddNodeStrings(TSharedPtr <SVerticalBox> VerticalBox)
{
	UDialogNode* DialogNode = Cast<UDialogNode>(NodeBace->CustomNode);

	VerticalBox->AddSlot()
		.AutoHeight()
		.Padding(0, 5)
		[
			SNew(STextBlock)
			.Text(UDialogNodeBase::GetDialogName(DialogNode->Dialogs[0]->Dialog))
			.Font(FSlateFontInfo(FPaths::EngineContentDir() / TEXT("Slate/Fonts/Roboto-Bold.ttf"), 16))

		];

	if (DialogNode->DialogOwner != ECharDialogOwner::NPC)
	{
		for (int i = 1; i < DialogNode->Dialogs.Num(); i++)
		{
			VerticalBox->AddSlot()
				.AutoHeight()
				.Padding(0, 5)
				[
					SNew(STextBlock)
					.Text(UDialogNodeBase::GetDialogName(DialogNode->Dialogs[i]->Dialog))
				.Font(FSlateFontInfo(FPaths::EngineContentDir() / TEXT("Slate/Fonts/Roboto-Bold.ttf"), 16))

				];
		}
	}
}

void SGraphNode_Dialog::CreatePinWidgets()
{
	UDialogNode* DialogNode = Cast<UDialogNode>(NodeBace->CustomNode);

	if (DialogNode->DialogOwner != ECharDialogOwner::NPC)
	{
		for (int i = 0; i < NodeBace->Pins.Num(); i++)
		{
			TSharedPtr<SGraphPin> NewPin = SNew(SGraphPin, NodeBace->Pins[i]);
			NewPin->SetIsEditable(IsEditable);
			AddPin(NewPin.ToSharedRef());

			if (NodeBace->Pins[i]->Direction == EEdGraphPinDirection::EGPD_Input)
			{
				InputPins.Add(NewPin.ToSharedRef());
			}
			else
			{
				OutputPins.Add(NewPin.ToSharedRef());
			}
		}
	}
	else
	{
		for (int i = 0; i < 2; i++)
		{
			TSharedPtr<SGraphPin> NewPin = SNew(SGraphPin, NodeBace->Pins[i]);
			NewPin->SetIsEditable(IsEditable);
			AddPin(NewPin.ToSharedRef());
			
			if (NodeBace->Pins[i]->Direction == EEdGraphPinDirection::EGPD_Input)
			{
				InputPins.Add(NewPin.ToSharedRef());
			}
			else
			{
				OutputPins.Add(NewPin.ToSharedRef());
			}

			
			
		}
	}

	

}

