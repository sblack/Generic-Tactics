// Fill out your copyright notice in the Description page of Project Settings.


#include "ActionAttack.h"
#include "CombatManager.h"
#include "../Movement/NavGrid.h"

void UActionAttack::Resolve(TScriptInterface<IActionSourceInterface> source, FVector target)
{
	Super::Resolve(source, target);

	TArray<FVector> targetArea = GetAffectedArea(source, target);

	for (int i = 0; i < targetArea.Num(); i++)
	{
		FGridData data = ANavGrid::Instance->GetGridData(targetArea[i]);
		//TODO: team/object check
		if (data.Occupant && data.Occupant->AsActor() != source->AsActor())
		{
			if (UCombatManager::RollAttack(this, source, data.Occupant))
			{
				data.Occupant->OnHit(false);
			}
		}
	}
}
