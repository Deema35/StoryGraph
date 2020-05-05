#include "ExecutionTree.h"

#include "CustomNodes.h"

void UExecutionTree::Refresh()
{
	TArray<UCustomNodeBase*> ChildNodes;

	TArray<UStoryVerticalNodeBase*> NodsForAdd;

	if (MainQuest->GetCurrentState() == (int)EQuestStates::Active ||
		MainQuest->GetCurrentState() == (int)EQuestStates::UnActive)
	{
		bool bAllNodesPerformed = false;

		while (!bAllNodesPerformed)
		{
			bAllNodesPerformed = true;

			for (int j = 0; j < ActiveNodesBuffer.Num(); j++)
			{
				if (UStoryVerticalNodeBase* VerticalNode = Cast<UStoryVerticalNodeBase>(ActiveNodesBuffer[j]))
				{
					EPerformNodeResult PerformResult = VerticalNode->PerformNode();

					if (PerformResult == EPerformNodeResult::Successed)
					{
						VerticalNode->ResetUnPerformBrunch();
						//if we have already perform node, we must reset others ways for this node
						VerticalNode->GetChildNodes(ChildNodes, EPinDataTypes::PinType_Vertical);
						if (ChildNodes.Num() == 0)
						{
							MainQuest->SetCurrentState((int)EQuestStates::Complete);
							//We find end of tree an quest complete
							break;
						}
						for (int i = 0; i < ChildNodes.Num(); i++)
						{
							NodsForAdd.Add((UStoryVerticalNodeBase*)ChildNodes[i]);
						}
						bAllNodesPerformed = false;
					}
					else if (PerformResult == EPerformNodeResult::Fail)
					{
						NodsForAdd.Add(ActiveNodesBuffer[j]);
					}
				}
			}

			ActiveNodesBuffer.Empty();
			ActiveNodesBuffer.Append(NodsForAdd);
			NodsForAdd.Empty();
		}
	}
}
