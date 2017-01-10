// Copyright 2016 Dmitriy Pavlov

#pragma once

#include "GameFramework/HUD.h"
#include "HUD_StoryGraph.generated.h"

/**
 * 
 */
class UDialogObject;

UCLASS()
class STORYGRAPHPLUGINRUNTIME_API AHUD_StoryGraph : public AHUD
{
	GENERATED_BODY()

public:
	TArray<class UStoryGraphObject*> CurrentStoryGraphObjects;

	class UGameScreen_StoryGraphWidget* GameScreen;

	class URadar_StoryGraphWidget* Radar;

private:
	TArray<class UStoryGraph*> StoryGraphs;
	bool StoryGraphFind;
public:
	/*Event perform when perform quest phase marked as "End game"*/
	UFUNCTION(BlueprintImplementableEvent, Category = StoryGraph)
		void EndGame(const FText& Decription);
	/*Event perform when perform quest phase marked as "Print on string"*/
	UFUNCTION(BlueprintImplementableEvent, Category = StoryGraph)
		void PrintQuestPhaseOnScreen(const FText& QuestPhaseDecription);
	/*Event perform when you engage in dialogue with a character or close dialog*/
	UFUNCTION(BlueprintImplementableEvent, Category = StoryGraph)
		void OpenDialogEvent(const bool IsOpen);
	/*Event perform when you use PlaceTrigger*/
	UFUNCTION(BlueprintImplementableEvent, Category = StoryGraph)
		void OpenPlaceTriggerMessagesEvent(const bool IsOpen);
	/* en - English ru - Russian ...*/
	UFUNCTION(BlueprintCallable, Category = StoryGraph)
		static void ChangeLocalization(FString target);

	UFUNCTION(BlueprintCallable, Category = StoryGraph)
		static FString  GetCurrentLocalization();
	
	void OpenDialogOrOpenPlaceTriggerMessages(TArray<class UStoryGraphObjectWithScenObject*>& OwningObjects);

	void GetRootDialogs(TArray<UDialogObject*>& Dialogs);


	static void GetNextDialogFromBrunch(class UCustomNodeBase* Brunch, TArray<UDialogObject*>& Dialogs);

	virtual void PreInitializeComponents() override;

	virtual void BeginPlay() override;

	void GetStoryGraphs(TArray<class UStoryGraph*>& StoryGraphs_);

};
