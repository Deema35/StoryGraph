// Copyright 2016 Dmitriy Pavlov
#include "SaveGameInstance.h"

#include "Runtime/Core/Public/Serialization/ArchiveLoadCompressedProxy.h"
#include "Runtime/Core/Public/Serialization/ArchiveSaveCompressedProxy.h"
#include "Engine/Engine.h"
#include "EngineUtils.h"
#include "HUD_StoryGraph.h"
#include "LogCategoryRutime.h"
#include "SaveObject_StoryGraph.h"
#include "StoryGraphWidget.h"

void USaveGameInstance::SaveGame()
{
	if (GetWorld())
	{
		TArray<FObjectRecord> ObjectRecordStoreSave;

		APawn* Pawn = GetWorld()->GetFirstPlayerController()->GetPawn();
		ObjectRecordStoreSave.Add(FObjectRecord(Pawn));

		for (TActorIterator<AActor> ActorItr(GetWorld()); ActorItr; ++ActorItr)
		{
			if (ISaveObject_StoryGraph* SaveObject = Cast<ISaveObject_StoryGraph>(*ActorItr))
			{
				ObjectRecordStoreSave.Add(FObjectRecord(*ActorItr));
			}
		}

		TArray<uint8> Data;
		FMemoryWriter MemoryWriter(Data, true);
		FSaveArchiveHeader ArchiveHeader;

		ArchiveHeader.ObjectRecordNum = ObjectRecordStoreSave.Num();
		FString MapName = GetWorld()->GetMapName();
		int NameStart = MapName.Find("_", ESearchCase::IgnoreCase, ESearchDir::FromEnd);

		ArchiveHeader.LevelName = MapName.RightChop(NameStart + 1);
		MemoryWriter << ArchiveHeader;

		for (int i = 0; i < ObjectRecordStoreSave.Num(); i++)
		{
			MemoryWriter << ObjectRecordStoreSave[i];
		}

		FString SavePath = FPaths::ProjectSavedDir() + FString("SaveGames/") + "quicksave.save";

		SaveToFileCompressed(SavePath, Data);

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
		IsLevelLoaded = true;

		LoadToFileCompressed(SavePath, Data);
		FMemoryReader MemoryReader(Data, true);

		FSaveArchiveHeader ArchiveHeader;
		MemoryReader << ArchiveHeader;


		UGameplayStatics::OpenLevel(GetWorld(), FName(*ArchiveHeader.LevelName), true);

		//Read object records

		for (int i = 0; i < ArchiveHeader.ObjectRecordNum; i++)
		{
			FObjectRecord ObjectRecord;
			MemoryReader << ObjectRecord;
			ObjectRecordStore.Add(ObjectRecord);
		}
	}
}

void USaveGameInstance::LoadGameContinue()
{
	if (IsLevelLoaded && GetWorld() && ObjectRecordStore.Num() > 0)
	{
		int i = 0;

		for (TActorIterator<AActor> ActorItr(GetWorld()); ActorItr; ++ActorItr)
		{
			if (ISaveObject_StoryGraph* SaveObject = Cast<ISaveObject_StoryGraph>(*ActorItr))
			{
				if (ObjectRecordStore.Num() > i)
				{
					bool ObjectFind = false;

					if (ObjectRecordStore[i].ObjectName == ActorItr->GetName())
					{
						ObjectRecordStore[i].Load(*ActorItr);
						ObjectFind = true;
					}
					else
					{
						for (int j = 0; j < ObjectRecordStore.Num(); j++)
						{
							if (ObjectRecordStore[j].ObjectName == ActorItr->GetName())
							{
								ObjectRecordStore[j].Load(*ActorItr);
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
						UE_LOG(LogCategoryStoryGraphPluginRuntime, Warning,
						       TEXT("Was find objec witch has not record in save file %s"), *ActorItr->GetName());
					}
				}
				else
				{
					UE_LOG(LogCategoryStoryGraphPluginRuntime, Warning,
					       TEXT("Object records in save file less then objects on map"));
					return;
				}
			}
		}


		// Load data from object records to objects
	}
}

void USaveGameInstance::LoadCharacter()
{
	if (IsLevelLoaded && GetWorld() && ObjectRecordStore.Num() > 0)
	{
		APawn* Pawn = GetWorld()->GetFirstPlayerController()->GetPawn();
		ObjectRecordStore[0].Load(Pawn);


		IsLevelLoaded = false;
		ObjectRecordStore.Empty();

		if (AHUD_StoryGraph* HUD = Cast<AHUD_StoryGraph>(GetWorld()->GetFirstPlayerController()->GetHUD()))
		{
			if (HUD->GameScreen)
			{
				HUD->GameScreen->AddMessageOnScreen(NSLOCTEXT("StoryGraph", "Game load", "Game load"), 5.0);
			}
		}
	}
}


bool USaveGameInstance::SaveToFileCompressed(FString SavePath, TArray<uint8>& Data)
{
	TArray<uint8> CompressedData;
	FArchiveSaveCompressedProxy Compressor(CompressedData,NAME_Zlib,ECompressionFlags::COMPRESS_ZLIB);
	// Compressed
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

bool USaveGameInstance::LoadToFileCompressed(FString SavePath, TArray<uint8>& Data)
{
	TArray<uint8> CompressedData;

	if (!FFileHelper::LoadFileToArray(CompressedData, *SavePath))
	{
		UE_LOG(LogCategoryStoryGraphPluginRuntime, Error, TEXT("Cann't open file"));

		CompressedData.Empty();
		return false;
	}
	// Decompress File 
	FArchiveLoadCompressedProxy Decompressor(CompressedData,NAME_Zlib,ECompressionFlags::COMPRESS_ZLIB);

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
