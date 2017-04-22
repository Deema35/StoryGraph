
#include "ObjectrRecord.h"
#include "CustomNods.h"
#include "LogCategoryRutime.h"

FObjectRecord::FObjectRecord(UObject* Object)
{
	
	ObjectName = Object->GetName();

	if (AActor* Actor = Cast<AActor>(Object))
	{
		ActorLocation = Actor->GetActorLocation();
		ActorRotation = Actor->GetActorRotation();
	}

	FMemoryWriter MemoryWriter(ObjectData, true);
	FObjectAndNameAsStringProxyArchive Ar(MemoryWriter, false);

	Ar.ArIsSaveGame = true;
	Ar.ArNoDelta = true;

	Object->Serialize(Ar);
	
	ISaveObject_StoryGraph* SaveObject = Cast<ISaveObject_StoryGraph>(Object);
	TArray<UObject*> Objects;

	if (SaveObject) SaveObject->GetInternallySaveObjects(Objects,0);
	
	for (int i = 0; i < Objects.Num(); i++)
	{
		InternallyObjects.Add(FObjectRecord(Objects[i]));
	}
	
}

void FObjectRecord::Load(UObject* Object)
{
	
	FMemoryReader MemoryReader(ObjectData, true);
	FObjectAndNameAsStringProxyArchive Ar(MemoryReader, false);

	if (AActor* Actor = Cast<AActor>(Object))
	{
		Actor->SetActorLocation(ActorLocation);
		Actor->SetActorRotation(ActorRotation);
	}

	Ar.ArIsSaveGame = true;
	Ar.ArNoDelta = true;

	Object->Serialize(Ar);
	
	
	TArray<UObject*> Objects;

	if (ISaveObject_StoryGraph* SaveObject = Cast<ISaveObject_StoryGraph>(Object))
	{
		SaveObject->GetInternallySaveObjects(Objects, InternallyObjects.Num());
	}

	if (InternallyObjects.Num() == Objects.Num())
	{

		for (int i = 0; i < Objects.Num(); i++)
		{
			InternallyObjects[i].Load(Objects[i]);
		}
	}
	else
	{
		UE_LOG(LogCategoryStoryGraphPluginRuntime, Warning, TEXT("Mismatch internaly objects and records num"));
	}
	
}


