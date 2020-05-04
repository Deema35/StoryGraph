#pragma once
#include "Interface.h"
#include "SaveObject_StoryGraph.generated.h"

UINTERFACE()
class USaveObject_StoryGraph : public UInterface
{
    GENERATED_BODY()
};

class ISaveObject_StoryGraph
{
    GENERATED_BODY()
public:
    bool IsLoaded;
public:
    virtual void GetInternallySaveObjects(TArray<UObject*>& Objects, int WantedObjectsNum)
    {
    } //if we load object we take function number objects witch we need
};
