#pragma once
#include "Object.h"
#include "Interface.h"
#include "ObjectrRecord.generated.h"



struct FSaveAchiveHeader
{
	int ObjectrRecordNum;
	FString LevelName;
};

struct FObjectRecord
{
	FObjectRecord() {}
	FObjectRecord(UObject* Object);

	FString ObjectName;
	FVector ActorLocation;
	FRotator ActorRotation;
	int32 InternallyObjectsNum;
	TArray<uint8> ObjectData;

	TArray<FObjectRecord> InternallyObjects;
	void Load(UObject* Object);
	
};


UINTERFACE()
class USaveObject_StoryGraph : public UInterface
{
	GENERATED_BODY()
};

class ISaveObject_StoryGraph
{
	GENERATED_BODY()
public:
	bool IsLoded;
public:
	virtual void GetInternallySaveObjects(TArray<UObject*>& Objects, int WantedObjectsNum) {} //if we load object we take function number objects witch we need
	
};



FORCEINLINE FArchive& operator<<(FArchive &Ar, FSaveAchiveHeader& AchiveHeader)
{
	Ar << AchiveHeader.ObjectrRecordNum;
	Ar << AchiveHeader.LevelName;
	return Ar;
}

FORCEINLINE FArchive& operator<<(FArchive &Ar, FObjectRecord& ObjectrRecord)
{
	Ar << ObjectrRecord.ObjectName;
	Ar << ObjectrRecord.ActorLocation;
	Ar << ObjectrRecord.ObjectData;
	Ar << ObjectrRecord.ActorRotation;
	if (Ar.IsLoading())
	{
		Ar << ObjectrRecord.InternallyObjectsNum;
		for (int i = 0; i < ObjectrRecord.InternallyObjectsNum; i++)
		{
			ObjectrRecord.InternallyObjects.Add(FObjectRecord());
			Ar << ObjectrRecord.InternallyObjects[i];
		}
	}
	else
	{
		ObjectrRecord.InternallyObjectsNum = ObjectrRecord.InternallyObjects.Num();
		Ar << ObjectrRecord.InternallyObjectsNum;
		for (int i = 0; i < ObjectrRecord.InternallyObjects.Num(); i++)
		{
			Ar << ObjectrRecord.InternallyObjects[i];
		}

	}

	return Ar;
}
