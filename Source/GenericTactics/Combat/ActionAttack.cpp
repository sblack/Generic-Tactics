// Fill out your copyright notice in the Description page of Project Settings.


#include "ActionAttack.h"
#include "CombatManager.h"
#include "../Movement/NavGrid.h"

void UActionAttack::Resolve(class AGTCharacter* user, FVector target)
{
	Super::Resolve(user, target);

	TArray<FVector> targetArea = GetAffectedArea(user, target);

	for (int i = 0; i < targetArea.Num(); i++)
	{
		FGridData data = ANavGrid::Instance->GetGridData(targetArea[i]);
		//TODO: team/object check
		if (data.Occupant && data.Occupant != user)
		{
			if(UCombatManager::RollAttack(this, user, data.Occupant))
			{
				data.Occupant->OnHit(false);
			}
		}
	}
}
