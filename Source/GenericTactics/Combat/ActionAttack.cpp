// Fill out your copyright notice in the Description page of Project Settings.


#include "ActionAttack.h"
#include "CombatManager.h"
#include "CombatEffect.h"
#include "../Movement/NavGrid.h"

void UActionAttack::Resolve(TScriptInterface<IActionSourceInterface> source, FVector target)
{
	Super::Resolve(source, target);

	TArray<FVector> targetArea = GetAffectedArea(source, target);

	for (int i = 0; i < targetArea.Num(); i++)
	{
		FGridData data = ANavGrid::Instance->GetGridData(targetArea[i]);
		UE_LOG(LogTemp, Log, TEXT("resolve at %s"), *data.Location.ToString());
		if (data.Occupant)
		{
			UE_LOG(LogTemp, Log, TEXT("   occupant: %s"), *data.Occupant->GetDisplayName().ToString());
		}
		else
		{
			UE_LOG(LogTemp, Log, TEXT("   occupant: EMPTY"));
		}
		//TODO: team/object check
		if (data.Occupant && data.Occupant->AsActor() != source->AsActor())
		{
			switch (UCombatManager::RollAttack(this, source, data.Occupant))
			{
				case ESuccessLevel::CritHit:
					data.Occupant->OnHit(true);
					UCombatEffect::ApplyEffectsCrit(data.Occupant, Effects, source);
					break;
				case ESuccessLevel::Hit:
					data.Occupant->OnHit(false);
					UCombatEffect::ApplyEffects(data.Occupant, Effects, source);
					break;
				case ESuccessLevel::Miss:
				default:
					break;
			}
		}
	}
}
