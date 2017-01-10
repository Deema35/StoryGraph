#pragma once
#include "Engine/GameInstance.h"
#include "ObjectrRecord.h"
#include "SaveGameInstance.generated.h"

UCLASS()
class STORYGRAPHPLUGINRUNTIME_API USaveGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	bool IsLevelLoded;


private:
	TArray<FObjectRecord> ObjectrRecordStore;

public:
	UFUNCTION(BlueprintCallable, Category = "SaveGame")
		void SaveGame();

	UFUNCTION(BlueprintCallable, Category = "SaveGame")
		void LoadGame();

	void LoadGameContinue();

	void LoadCharacter();

	static bool SaveToFileCompresed(FString SavePath, TArray<uint8>& Data);
	static bool LoadToFileCompresed(FString SavePath, TArray<uint8>& Data);
	static bool SaveToFile(FString SavePath, TArray<uint8>& Data);
	static bool LoadToFile(FString SavePath, TArray<uint8>& Data);
};
