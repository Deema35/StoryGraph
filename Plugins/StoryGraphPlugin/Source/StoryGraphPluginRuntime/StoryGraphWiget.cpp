// Copyright 2016 Dmitriy Pavlov
#pragma warning(disable: 4458)


#include "StoryGraphWiget.h"
#include "StoryGraph.h"
#include "StoryGraphObject.h"
#include "CustomNods.h"
#include "Runtime/UMG/Public/Blueprint/WidgetBlueprintLibrary.h"
#include "HUD_StoryGraph.h"
#include "StoryScenObject.h"
#include "LogCategoryRutime.h"




//UJurnal_StoryGraphWidget......................................................................................

void UJurnal_StoryGraphWidget::GetQuests(TArray<class UStoryGraphQuest*>& Quests)
{
	
	if (AHUD_StoryGraph* HUD = Cast<AHUD_StoryGraph>(GetWorld()->GetFirstPlayerController()->GetHUD()))
	{
		TArray<UStoryGraph*> StoryGraphs;
		HUD->GetStoryGraphs(StoryGraphs);

		for (int i = 0; i < StoryGraphs.Num(); i++)
		{
			for (int j = 0; j < StoryGraphs[i]->GarphObjects.Num(); j++)
			{
				UStoryGraphQuest* Quest = Cast<UStoryGraphQuest>(StoryGraphs[i]->GarphObjects[j]);

				if (Quest && Quest->GetCurentState() != (int)EQuestStates::UnActive && Quest->QuestPhase.Num() > 0)
				{
					Quests.Add(Quest);
				}

			}

		}
	}

}

//UGameScreen_StoryGraphWidget...............................................................................
void UGameScreen_StoryGraphWidget::NativeConstruct()
{
	Super::NativeConstruct();
		
	if (AHUD_StoryGraph* HUD = Cast<AHUD_StoryGraph>(GetWorld()->GetFirstPlayerController()->GetHUD()))
	{
		if (!IsIniciate)
		{
			TArray<UStoryGraph*> StoryGraphs;
			HUD->GameScreen = this;
			IsIniciate = true;
		}
	}
}

void UGameScreen_StoryGraphWidget::AddMessageOnScreen(FText Text, float Duration)
{
	FTimerHandle UniqueHandle;
	FTimerDelegate TimerDelegate = FTimerDelegate::CreateUObject(this, &UGameScreen_StoryGraphWidget::RemoveMessageFromScreen, Text);
	if (GetWorld()) GetWorld()->GetTimerManager().SetTimer(UniqueHandle, TimerDelegate, Duration, false);
	
	AddMessageOnScreenEvent(Text);
}

void UGameScreen_StoryGraphWidget::RemoveMessageFromScreen(FText Text)
{
	RemoveMessageFromScreenEvent(Text);
}

void UGameScreen_StoryGraphWidget::AddDefaultAnswerOnScreen(FText Text)
{
	FTimerHandle UniqueHandle;
	if (GetWorld() && !DefaulAnswerOnScreen)
	{
		FTimerDelegate TimerDelegate = FTimerDelegate::CreateUObject(this, &UGameScreen_StoryGraphWidget::RemoveDefaultAnswerFromScreen);
		
		GetWorld()->GetTimerManager().SetTimer(UniqueHandle, TimerDelegate, 4.0, false);
		DefaulAnswerOnScreen = true;
		AddDefaultAnswerOnScreenEvent(Text);
	}
}

void UGameScreen_StoryGraphWidget::RemoveDefaultAnswerFromScreen()
{
	RemoveDefaultAnswerFromScreenEvent();
	DefaulAnswerOnScreen = false;
}

//URadar_StoryGraphWidget.........................................................................

void URadar_StoryGraphWidget::NativeConstruct()
{
	Super::NativeConstruct();

	Character = GetWorld() ? GetWorld()->GetFirstPlayerController()->GetPawn() : NULL;
	if (AHUD_StoryGraph* HUD = Cast<AHUD_StoryGraph>(GetWorld()->GetFirstPlayerController()->GetHUD()))
	{
		if (!IsIniciate)
		{
			TArray<UStoryGraph*> StoryGraphs;
			HUD->Radar = this;
			IsIniciate = true;
		}
	}

}

void URadar_StoryGraphWidget::RefreshTargets(TArray<class UQuestPhase*>& RadarTargets_)
{
	RadarTargets.Empty();
	RadarTargets.Append(RadarTargets_);
}

void URadar_StoryGraphWidget::NativePaint(FPaintContext& InContext) const
{
	
	if (!Character) return;
		
	FLinearColor MarkColor;
	
	for (int i = 0; i < RadarTargets.Num(); i++)
	{

		for (int j = 0; j < RadarTargets[i]->PhaseObjects.Num(); j++)
		{
			
			MarkColor = RadarTargets[i]->pOwnedQuest->MainQuest ? MainQuestMarkColor : OptionalQuestMarkColor;
			DrawRadarMark(InContext, MarkColor, RadarTargets[i]->PhaseObjects[j]);
		}
	}
}

void URadar_StoryGraphWidget::DrawRadarMark(FPaintContext& InContext, FLinearColor MarkColor, AActor* Target) const
{
	float Distance = GetXYDistance(Target->GetActorLocation(), Character->GetActorLocation());
	float DistanceMark = Distance > RadarVisionDistance ? RadarVisionDistance : Distance;
	
	float Angle = GetXYAngle(Character->GetController()->GetControlRotation().Vector(),Character->GetActorLocation() - Target->GetActorLocation());
	
	FVector MarkVector = FVector(0,0,0);
	MarkVector.Y = DistanceMark / RadarVisionDistance * (RadarWidgetSize / 2 - 5);
	
	MarkVector = MarkVector.RotateAngleAxis(Angle, FVector(0, 0, 1)); //Rotate vector in current angle
	MarkVector = MarkVector + FVector(RadarWidgetSize / 2, RadarWidgetSize / 2, 0);
	
	FVector2D A;
	FVector2D B;

	A.X = MarkVector.X - MarkLenght / 2;
	A.Y = MarkVector.Y - MarkLenght / 2;
	B.X = MarkVector.X + MarkLenght / 2;
	B.Y = MarkVector.Y + MarkLenght / 2;
	//A = FVector2D(RadarWidgetSize / 2, RadarWidgetSize / 2);
	//B = FVector2D(MarkVector.X, MarkVector.Y);
	UWidgetBlueprintLibrary::DrawLine(InContext, A, B, MarkColor);

	
	A.X = MarkVector.X - MarkLenght / 2;
	A.Y = MarkVector.Y + MarkLenght / 2;
	B.X = MarkVector.X + MarkLenght / 2;
	B.Y = MarkVector.Y - MarkLenght / 2;
	UWidgetBlueprintLibrary::DrawLine(InContext, A, B, MarkColor);

	
}

float URadar_StoryGraphWidget::GetXYDistance(FVector V1, FVector V2)
{
	return pow(pow(V1.X - V2.X, 2) + pow(V1.Y - V2.Y, 2), 0.5);
}

float URadar_StoryGraphWidget::GetXYAngle(FVector V1, FVector V2)
{
	float V1Len = pow(pow(V1.X, 2) + pow(V1.Y, 2), 0.5);
	float V2Len = pow(pow(V2.X, 2) + pow(V2.Y, 2), 0.5);

	float ScalarProduct = (V1.X * V2.X + V1.Y * V2.Y);
	float CosVek = ScalarProduct / (V1Len * V2Len);
	float B = V1.X * V2.Y - V2.X * V1.Y;
	float Sign = B > 0 ? 1 : -1;
	return acos(CosVek)  * 180.0 / PI * Sign;
}

//UDialog_StoryGraphWidget........................................................................

ECharDialogOwner UDialog_StoryGraphWidget::GetDialogs(const UDialogObject* CurrentDialog, TArray<UDialogObject*>& Dialogs)
{
	AHUD_StoryGraph* HUD = Cast<AHUD_StoryGraph>(GetWorld()->GetFirstPlayerController()->GetHUD());

	if (HUD)
	{
		Dialogs.Empty();

		if (CurrentDialog)
		{
			UDialogNodeBase* NextNode = NULL;
			((UDialogNodeBase*)CurrentDialog->DialogNode)->GetChildNode(CurrentDialog, NextNode);
			AHUD_StoryGraph::GetNextDialogFromBrunch(NextNode, Dialogs);
			if (Dialogs.Num() > 0)
			{
				return ((UDialogNodeBase*)Dialogs[0]->DialogNode)->DialogOwner;
			}
			else
			{
				HUD->GetRootDialogs(Dialogs);
				return ECharDialogOwner::Character;
			}
		}
		else
		{
			HUD->GetRootDialogs(Dialogs);
			return ECharDialogOwner::Character;
		}
	}
	else
	{
		UE_LOG(LogCategoryStoryGraphPluginRuntime, Warning, TEXT("Your HUD should inherit AHUD_StoryGraph class"));
	}
	return ECharDialogOwner::NotDefine;
}

FText UDialog_StoryGraphWidget::GetCharacterName()
{
	AHUD_StoryGraph* HUD = Cast<AHUD_StoryGraph>(GetWorld()->GetFirstPlayerController()->GetHUD());

	return HUD->CurrentStoryGraphObjects.Num() > 0 ? HUD->CurrentStoryGraphObjects[0]->ObjName : FText::FromString("Non");
}

//UInventory_StoryGraphWidget.......................................................

void UInventory_StoryGraphWidget::GetStoryInventoryItems(TArray<UStoryGraphInventoryItem*>& InventoryItems)
{
	TArray<UStoryGraph*> StoryGraphs;
	if (AHUD_StoryGraph* HUD = Cast<AHUD_StoryGraph>(GetWorld()->GetFirstPlayerController()->GetHUD()))
	{
		HUD->GetStoryGraphs(StoryGraphs);
		TArray<IStoryScenObject*> ObjectFilterData;

		for (int i = 0; i < StoryGraphs.Num(); i++)
		{
			for (int j = 0; j < StoryGraphs[i]->GarphObjects.Num(); j++)
			{
				UStoryGraphInventoryItem* InventoryItem = Cast<UStoryGraphInventoryItem>(StoryGraphs[i]->GarphObjects[j]);
				if (InventoryItem &&
					InventoryItem->IsScenObjectActive &&
					InventoryItem->GetCurentState() != (int)EInventoryItemeStates::OnLevel + 1)
				{
					if (ObjectFilter(InventoryItem, ObjectFilterData))
					{
						InventoryItems.Add(InventoryItem);
					}
				}

			}

		}
	}
}

bool UInventory_StoryGraphWidget::ObjectFilter(UStoryGraphInventoryItem* InventoryItem, TArray<IStoryScenObject*>& ObjectFilterData)
{
	TArray<IStoryScenObject*> ScenObjects;
	TArray<IStoryScenObject*> NewFilterData;

	InventoryItem->GetScenObjects(ScenObjects);

	for (int i = 0; i < ScenObjects.Num(); i++)
	{
		for (int j = 0; j < ObjectFilterData.Num(); j++)
		{
			if (ScenObjects[i] == ObjectFilterData[j])
			{
				
				return false;
			}
			
		}
		
		NewFilterData.Add(ScenObjects[i]);
	}

	ObjectFilterData.Append(NewFilterData);

	return true;

}