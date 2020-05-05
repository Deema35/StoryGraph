#pragma once
#include "CoreMinimal.h"

struct FSaveArchiveHeader
{
	int ObjectRecordNum;
	FString LevelName;
};

struct FObjectRecord
{
	FObjectRecord()
	{
	}

	FObjectRecord(UObject* Object);

	FString ObjectName;
	FVector ActorLocation;
	FRotator ActorRotation;
	int32 InternallyObjectsNum;
	TArray<uint8> ObjectData;

	TArray<FObjectRecord> InternallyObjects;
	void Load(UObject* Object);
};

FORCEINLINE FArchive& operator<<(FArchive& Ar, FSaveArchiveHeader& ArchiveHeader)
{
	Ar << ArchiveHeader.ObjectRecordNum;
	Ar << ArchiveHeader.LevelName;
	return Ar;
}

FORCEINLINE FArchive& operator<<(FArchive& Ar, FObjectRecord& ObjectRecord)
{
	Ar << ObjectRecord.ObjectName;
	Ar << ObjectRecord.ActorLocation;
	Ar << ObjectRecord.ObjectData;
	Ar << ObjectRecord.ActorRotation;
	if (Ar.IsLoading())
	{
		Ar << ObjectRecord.InternallyObjectsNum;
		for (int i = 0; i < ObjectRecord.InternallyObjectsNum; i++)
		{
			ObjectRecord.InternallyObjects.Add(FObjectRecord());
			Ar << ObjectRecord.InternallyObjects[i];
		}
	}
	else
	{
		ObjectRecord.InternallyObjectsNum = ObjectRecord.InternallyObjects.Num();
		Ar << ObjectRecord.InternallyObjectsNum;
		for (int i = 0; i < ObjectRecord.InternallyObjects.Num(); i++)
		{
			Ar << ObjectRecord.InternallyObjects[i];
		}
	}

	return Ar;
}
