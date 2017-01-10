// Copyright 2016 Dmitriy Pavlov
#pragma once


#include "Runtime/UMG/Public/Blueprint/UserWidget.h"
#include "StoryGraphWiget.generated.h"

enum class ECharDialogOwner : uint8;

UCLASS(Abstract)
class UBase_StoryGraphWiget : public UUserWidget
{
	GENERATED_BODY()

};

UCLASS(Blueprintable)
class UJurnal_StoryGraphWidget : public UBase_StoryGraphWiget
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, Category = StoryGraph)
	void GetQuests(TArray<class UStoryGraphQuest*>& Quests);

};

UCLASS(Blueprintable)
class UGameScreen_StoryGraphWidget : public UBase_StoryGraphWiget
{
	GENERATED_BODY()

private:

	bool IsIniciate;

	bool DefaulAnswerOnScreen;

public:
	virtual void NativeConstruct() override;

	UFUNCTION(BlueprintImplementableEvent, Category = StoryGraph)
		void AddMessageOnScreenEvent(const FText& Text);

	UFUNCTION(BlueprintImplementableEvent, Category = StoryGraph)
		void RemoveMessageFromScreenEvent(const FText& Text);

	UFUNCTION(BlueprintImplementableEvent, Category = StoryGraph)
		void AddDefaultAnswerOnScreenEvent(const FText& Text);

	UFUNCTION(BlueprintImplementableEvent, Category = StoryGraph)
		void RemoveDefaultAnswerFromScreenEvent();

	void AddMessageOnScreen(FText Text, float Duration);

	void RemoveMessageFromScreen(FText Text);

	void AddDefaultAnswerOnScreen(FText Text);

	void RemoveDefaultAnswerFromScreen();

};

UCLASS(Blueprintable)
class URadar_StoryGraphWidget : public UBase_StoryGraphWiget
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		float RadarVisionDistance;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
		float RadarWidgetSize;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		float MarkLenght;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		FLinearColor MainQuestMarkColor;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		FLinearColor OptionalQuestMarkColor;


private:
	bool IsIniciate;

	TArray<class UQuestPhase*> RadarTargets;

	APawn* Character;

public:
	URadar_StoryGraphWidget() : RadarVisionDistance(2000), RadarWidgetSize(256), MarkLenght(12), MainQuestMarkColor(FLinearColor::Red), OptionalQuestMarkColor(FLinearColor::Blue){}

	virtual void NativeConstruct() override;

	void RefreshTargets(TArray<class UQuestPhase*>& RadarTargets_);

	virtual void NativePaint(FPaintContext& InContext) const override;

private:

	void DrawRadarMark(FPaintContext& InContext, FLinearColor MarkColor, AActor* Target) const;

	static float GetXYDistance(FVector V1, FVector V2);

	static float GetXYAngle(FVector V1, FVector V2);
};

UCLASS(Blueprintable)
class UDialog_StoryGraphWidget : public UBase_StoryGraphWiget
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, Category = StoryGraph)
	ECharDialogOwner GetDialogs(const class UDialogObject* CurrentDialog, TArray<UDialogObject*>& Dialogs);

	UFUNCTION(BlueprintPure, Category = StoryGraph)
		FText GetCharacterName();
};

UCLASS(Blueprintable)
class UInventory_StoryGraphWidget : public UBase_StoryGraphWiget
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, Category = StoryGraph)
		void GetStoryInventoryItems(TArray<UStoryGraphInventoryItem*>& InventoryItems);

private:

	bool ObjectFilter(UStoryGraphInventoryItem* InventoryItem, TArray<class IStoryScenObject*>& ObjectFilterData);

};

