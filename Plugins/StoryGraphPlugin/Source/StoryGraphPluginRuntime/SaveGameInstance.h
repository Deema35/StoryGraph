#pragma once
#include "Engine/GameInstance.h"
#include "ObjectRecord.h"
#include "SaveGameInstance.generated.h"

UCLASS()
class STORYGRAPHPLUGINRUNTIME_API USaveGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	bool IsLevelLoaded;

private:
	TArray<FObjectRecord> ObjectRecordStore;

public:
	UFUNCTION(BlueprintCallable, Category = "SaveGame")
	void SaveGame();

	UFUNCTION(BlueprintCallable, Category = "SaveGame")
	void LoadGame();

	void LoadGameContinue();

	void LoadCharacter();

	static bool SaveToFileCompressed(FString SavePath, TArray<uint8>& Data);
	static bool LoadToFileCompressed(FString SavePath, TArray<uint8>& Data);
	static bool SaveToFile(FString SavePath, TArray<uint8>& Data);
	static bool LoadToFile(FString SavePath, TArray<uint8>& Data);
};
