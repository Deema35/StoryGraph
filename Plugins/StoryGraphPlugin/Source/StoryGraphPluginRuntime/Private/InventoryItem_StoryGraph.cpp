#include "InventoryItem_StoryGraph.h"

void AInventoryItem_StoryGraph::PickUp()
{
    SetStoryGraphObjectState((int)EInventoryItemStates::InInventory + 1);

    EnableObjectOnMap(false);
}


void AInventoryItem_StoryGraph::RefreshSceneObjectsActive()
{
    bool IsActive = false;

    for (int i = 0; i < OwningStoryGraphObject.Num(); i++)
    {
        if (OwningStoryGraphObject[i]->IsSceneObjectActive)
        {
            IsActive = true;
            break;
        }
    }
    if (IsActive && OwningStoryGraphObject[0]->GetCurrentState() != (int)EInventoryItemStates::OnLevel + 1)
    {
        IsActive = false;
    }
    EnableObjectOnMap(IsActive);
}
