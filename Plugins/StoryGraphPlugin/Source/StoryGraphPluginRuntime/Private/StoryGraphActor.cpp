#include "StoryGraphActor.h"

#include "UObject/Package.h"
#include "StoryGraph.h"
#include "StoryGraphBlueprint.h"
#include "StoryGraphObject.h"
#include "StorySceneObject.h"
#include "SaveGameInstance.h"


AStoryGraphActor::AStoryGraphActor()
{
	StoryGraph = nullptr;
}


bool AStoryGraphActor::CreateStoryGraph()
{
	if ((UStoryGraphBlueprint*)GetClass()->ClassGeneratedBy)
	{
		if (!StoryGraph || StoryGraph->CompilationCounter != ((UStoryGraphBlueprint*)GetClass()->ClassGeneratedBy)
		                                                     ->StoryGraph->CompilationCounter)
		{
			StoryGraph = DuplicateObject<UStoryGraph>(((UStoryGraphBlueprint*)GetClass()->ClassGeneratedBy)->StoryGraph,
			                                          this);
			if (StoryGraph)
			{
				for (int i = 0; i < StoryGraph->GraphObjects.Num(); i++)
				{
					UStoryGraphObjectWithSceneObject* ObjectWithSceneObject = Cast<UStoryGraphObjectWithSceneObject>(
						StoryGraph->GraphObjects[i]);
					if (ObjectWithSceneObject)
					{
						ObjectWithSceneObject->SetSceneObjectRealPointers();
					}
				}

				CompilationCounter = StoryGraph->CompilationCounter;
				return true;
			}
		}
	}

	return false;
}

void AStoryGraphActor::GetInternallySaveObjects(TArray<UObject*>& Objects, int WantedObjectsNum)
{
	if (StoryGraph)
	{
		Objects.Add(StoryGraph);
	}
}

void AStoryGraphActor::ClearCrossLevelReferences()
{
	Super::ClearCrossLevelReferences();
	CreateStoryGraph();
}

void AStoryGraphActor::PreInitializeComponents()
{
	Super::PreInitializeComponents();


	if (StoryGraph)
	{
		StoryGraph->OwnedActor = this;

		for (int i = 0; i < StoryGraph->GraphObjects.Num(); i++)
		{
			if (UStoryGraphObjectWithSceneObject* ObjectWithSceneObject = Cast<UStoryGraphObjectWithSceneObject>(
				StoryGraph->GraphObjects[i]))
			{
				TArray<IStorySceneObject*> SceneObjects;
				ObjectWithSceneObject->GetSceneObjects(SceneObjects);

				for (int j = 0; j < SceneObjects.Num(); j++)
				{
					SceneObjects[j]->OwningStoryGraphObject.Add(
						(UStoryGraphObjectWithSceneObject*)StoryGraph->GraphObjects[i]);
				}
			}
		}

		USaveGameInstance* SaveGameInstance = Cast<USaveGameInstance>(GetGameInstance());

		if (!(SaveGameInstance && SaveGameInstance->IsLevelLoaded))
		{
			StoryGraph->CreateExecutionTrees();
		}
	}
}


void AStoryGraphActor::BeginPlay()
{
	Super::BeginPlay();

	for (int i = 0; i < StoryGraph->GraphObjects.Num(); i++)
	{
		if (UStoryGraphObjectWithSceneObject* ObjectWithSceneObject = Cast<UStoryGraphObjectWithSceneObject>(
			StoryGraph->GraphObjects[i]))
		{
			ObjectWithSceneObject->InitializeObject(); //Set initial active state
		}
	}

	StoryGraph->RefreshExecutionTrees();
}

void AStoryGraphActor::Serialize(FArchive& Ar)
{
#if WITH_EDITOR
	if (Ar.IsSaving() && (Ar.GetPortFlags() & PPF_DuplicateForPIE))
	{
		if (CreateStoryGraph())
		{
			MarkPackageDirtyCustom();
		}
	}
	/*if (Ar.IsLoading() && (Ar.GetPortFlags() & PPF_DuplicateForPIE))
	{
		
		
	}*/
#endif

	Super::Serialize(Ar);
}

void AStoryGraphActor::MarkPackageDirtyCustom() const
{
	UPackage* Package = GetOutermost();

	if (Package != nullptr)
	{
		const bool bIsDirty = Package->IsDirty();

		if (!bIsDirty)
		{
			Package->SetDirtyFlag(true);
		}

		Package->PackageMarkedDirtyEvent.Broadcast(Package, bIsDirty);
	}
}