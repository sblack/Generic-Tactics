// Fill out your copyright notice in the Description page of Project Settings.


#include "AIObjective.h"
#include "GTCharacter.h"
#include "../Combat/CombatManager.h"
#include "../Movement/NavGrid.h"

TArray<class AGTCharacter*> UAIObjective::GetAllEnemiesInRange(class AGTCharacter* character, float range)
{
	UE_LOG(LogTemp, Log, TEXT("GetAllEnemiesInRange range = %f"), range);
	TArray<AGTCharacter*> results;
	TArray<AGTCharacter*> otherTeam = UCombatManager::GetTeam(!character->IsPartyCharacter());
	for (AGTCharacter* otherChar : otherTeam)
	{
		FVector diff = character->GetActorLocation() - otherChar->GetActorLocation();
		float dist = ANavGrid::Instance->GetDistance(diff);
		if (dist <= range)
		{
			results.Add(otherChar);
			//UE_LOG(LogTemp, Log, TEXT("   %s at %f (%s)"), *otherChar->GetDisplayName().ToString(), dist, *diff.ToCompactString());
		}
	}

	return results;
}
