// Copyright 2016 Dmitriy Pavlov
#pragma once


#include "Runtime/UMG/Public/Blueprint/UserWidget.h"
#include "StoryGraphWidget.generated.h"

class UStoryGraphInventoryItem;
enum class ECharDialogOwner : uint8;

UCLASS(Abstract)
class UBase_StoryGraphWidget : public UUserWidget
{
	GENERATED_BODY()
};

UCLASS(Blueprintable)
class UJournal_StoryGraphWidget : public UBase_StoryGraphWidget
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, Category = StoryGraph)
	void GetQuests(TArray<class UStoryGraphQuest*>& Quests) const;
};

UCLASS(Blueprintable)
class UGameScreen_StoryGraphWidget : public UBase_StoryGraphWidget
{
	GENERATED_BODY()

private:

	bool IsInitiated;

	bool IsDefaultAnswerOnScreen;

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
class URadar_StoryGraphWidget : public UBase_StoryGraphWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float RadarVisionDistance;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float RadarWidgetSize;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float MarkLength;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FLinearColor MainQuestMarkColor;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FLinearColor OptionalQuestMarkColor;


private:
	bool IsInitiated;

	TArray<class UQuestPhase*> RadarTargets;

	UPROPERTY()
	APawn* Character;

public:
	URadar_StoryGraphWidget() : RadarVisionDistance(2000), RadarWidgetSize(256), MarkLength(12),
	                            MainQuestMarkColor(FLinearColor::Red), OptionalQuestMarkColor(FLinearColor::Blue)
	{
	}

	virtual void NativeConstruct() override;

	void RefreshTargets(TArray<class UQuestPhase*>& RadarTargets_);

	virtual int32 NativePaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled ) const override;

private:

	void DrawRadarMark(FPaintContext& InContext, FLinearColor MarkColor, AActor* Target) const;

	static float GetXYDistance(FVector V1, FVector V2);

	static float GetXYAngle(FVector V1, FVector V2);
};

UCLASS(Blueprintable)
class UDialog_StoryGraphWidget : public UBase_StoryGraphWidget
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, Category = StoryGraph)
	ECharDialogOwner GetDialogs(const class UDialogObject* CurrentDialog, TArray<UDialogObject*>& Dialogs) const;

	UFUNCTION(BlueprintPure, Category = StoryGraph)
	FText GetCharacterName() const;
};

UCLASS(Blueprintable)
class UInventory_StoryGraphWidget : public UBase_StoryGraphWidget
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, Category = StoryGraph)
	void GetStoryInventoryItems(TArray<UStoryGraphInventoryItem*>& InventoryItems) const;

private:

	static bool ObjectFilter(UStoryGraphInventoryItem* InventoryItem,
	                         TArray<class IStorySceneObject*>& ObjectFilterData);
};
