// Copyright 2016 Dmitriy Pavlov


#include "SaveGameInstance.h"
#include "HUD_StoryGraph.h"
#include "LogCategoryRutime.h"
#include "Engine/Engine.h"
#include "EngineUtils.h"
#include "StoryGraphWiget.h"

void USaveGameInstance::SaveGame()
{
	
	if (GetWorld())
	{
		
		TArray<FObjectRecord> ObjectrRecordStoreSave;

		APawn* Pawn = GetWorld()->GetFirstPlayerController()->GetPawn();
		ObjectrRecordStoreSave.Add(FObjectRecord(Pawn));
		
		for (TActorIterator<AActor> ActorItr(GetWorld()); ActorItr; ++ActorItr)
		{
			if (ISaveObject_StoryGraph* SaveObject = Cast<ISaveObject_StoryGraph>(*ActorItr))
			{

				ObjectrRecordStoreSave.Add(FObjectRecord(*ActorItr));
			}
		}

		TArray<uint8> Data;
		FMemoryWriter MemoryWriter(Data, true);
		FSaveAchiveHeader  AchiveHeader;

		AchiveHeader.ObjectrRecordNum = ObjectrRecordStoreSave.Num();
		FString MapName = GetWorld()->GetMapName();
		int NameStart = MapName.Find("_", ESearchCase::IgnoreCase, ESearchDir::FromEnd);

		AchiveHeader.LevelName = MapName.RightChop(NameStart + 1);
		MemoryWriter << AchiveHeader;

		for (int i = 0; i < ObjectrRecordStoreSave.Num(); i++)
		{
			MemoryWriter << ObjectrRecordStoreSave[i];
		}

		FString SavePath = FPaths::ProjectSavedDir() + FString("SaveGames/") + "quicksave.save";

		SaveToFileCompresed(SavePath, Data);

		if (AHUD_StoryGraph* HUD = Cast<AHUD_StoryGraph>(GetWorld()->GetFirstPlayerController()->GetHUD()))
		{
			if (HUD->GameScreen)
			{
				HUD->GameScreen->AddMessageOnScreen(NSLOCTEXT("StoryGraph", "Game save", "Game save"), 5.0);
			}
		}
	}

}

void USaveGameInstance::LoadGame()
{
	if (GetWorld())
	{
		TArray<uint8> Data;
		FString SavePath = FPaths::ProjectSavedDir() + FString("SaveGames/") + "quicksave.save";
		IsLevelLoded = true;

		LoadToFileCompresed(SavePath, Data);
		FMemoryReader MemoryReader(Data, true);

		FSaveAchiveHeader  AchiveHeader;
		MemoryReader << AchiveHeader;



		UGameplayStatics::OpenLevel(GetWorld(), FName(*AchiveHeader.LevelName), true);

		//Read object records
		
		for (int i = 0; i < AchiveHeader.ObjectrRecordNum; i++)
		{
			FObjectRecord ObjectrRecord;
			MemoryReader << ObjectrRecord;
			ObjectrRecordStore.Add(ObjectrRecord);
		}

		
	}

}

void USaveGameInstance::LoadGameContinue()
{

	if (IsLevelLoded && GetWorld() && ObjectrRecordStore.Num() > 0)
	{

		int i = 0;

		for (TActorIterator<AActor> ActorItr(GetWorld()); ActorItr; ++ActorItr)
		{

			if (ISaveObject_StoryGraph* SaveObject = Cast<ISaveObject_StoryGraph>(*ActorItr))
			{
				
				if (ObjectrRecordStore.Num() > i)
				{
					bool ObjectFind = false;

					if (ObjectrRecordStore[i].ObjectName == ActorItr->GetName())
					{
						ObjectrRecordStore[i].Load(*ActorItr);
						ObjectFind = true;
					}
					else
					{
						for (int j = 0; j < ObjectrRecordStore.Num(); j++)
						{
							if (ObjectrRecordStore[j].ObjectName == ActorItr->GetName())
							{
								ObjectrRecordStore[j].Load(*ActorItr);
								ObjectFind = true;
							}
						}
						
					}

					if (ObjectFind)
					{
						i++;
					}
					else
					{
						UE_LOG(LogCategoryStoryGraphPluginRuntime, Warning, TEXT("Was find objec witch has not record in save file %s"), *ActorItr->GetName());
					}
				}
				else
				{
					UE_LOG(LogCategoryStoryGraphPluginRuntime, Warning, TEXT("Object records in save file less then objects on map"));
					return;
				}

			}
		}

		
		// Load data from object records to objects

		
	}
	
	
}

void USaveGameInstance::LoadCharacter()
{
	if (IsLevelLoded && GetWorld() && ObjectrRecordStore.Num() > 0)
	{
		APawn* Pawn = GetWorld()->GetFirstPlayerController()->GetPawn();
		ObjectrRecordStore[0].Load(Pawn);


		IsLevelLoded = false;
		ObjectrRecordStore.Empty();

		if (AHUD_StoryGraph* HUD = Cast<AHUD_StoryGraph>(GetWorld()->GetFirstPlayerController()->GetHUD()))
		{
			if (HUD->GameScreen)
			{
				HUD->GameScreen->AddMessageOnScreen(NSLOCTEXT("StoryGraph", "Game load", "Game load"), 5.0);
			}
		}
	}
}


bool USaveGameInstance::SaveToFileCompresed(FString SavePath, TArray<uint8>& Data)
{

	TArray<uint8> CompressedData;
	FArchiveSaveCompressedProxy Compressor(CompressedData, ECompressionFlags::COMPRESS_ZLIB);
	// Compresed
	Compressor << Data;
	//send archive serialized data to binary array
	Compressor.Flush();

	if (!FFileHelper::SaveArrayToFile(CompressedData, *SavePath))
	{
		UE_LOG(LogCategoryStoryGraphPluginRuntime, Error, TEXT("Cann't save file"));

		Compressor.FlushCache();
		CompressedData.Empty();
		return false;

	}

	Compressor.FlushCache();
	CompressedData.Empty();
	return true;
}

bool USaveGameInstance::LoadToFileCompresed(FString SavePath, TArray<uint8>& Data)
{
	TArray<uint8> CompressedData;

	if (!FFileHelper::LoadFileToArray(CompressedData, *SavePath))
	{
		UE_LOG(LogCategoryStoryGraphPluginRuntime, Error, TEXT("Cann't open file"));

		CompressedData.Empty();
		return false;
	}
	// Decompress File 
	FArchiveLoadCompressedProxy Decompressor(CompressedData, ECompressionFlags::COMPRESS_ZLIB);

	//Decompression Error?
	if (Decompressor.GetError())
	{
		UE_LOG(LogCategoryStoryGraphPluginRuntime, Error, TEXT("File Was Not Compressed"));

		Decompressor.FlushCache();
		CompressedData.Empty();
		return false;

	}

	//Decompress
	Decompressor << Data;

	Decompressor.FlushCache();
	CompressedData.Empty();

	return true;
}

bool USaveGameInstance::SaveToFile(FString SavePath, TArray<uint8>& Data)
{

	if (!FFileHelper::SaveArrayToFile(Data, *SavePath))
	{
		UE_LOG(LogCategoryStoryGraphPluginRuntime, Error, TEXT("Cann't save file"));
		return false;

	}

	return true;
}

bool USaveGameInstance::LoadToFile(FString SavePath, TArray<uint8>& Data)
{


	if (!FFileHelper::LoadFileToArray(Data, *SavePath))
	{
		UE_LOG(LogCategoryStoryGraphPluginRuntime, Error, TEXT("Cann't open file"));
		return false;
	}

	return true;
}