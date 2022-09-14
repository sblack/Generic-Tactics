// Fill out your copyright notice in the Description page of Project Settings.


#include "Action.h"
//#include "CombatManager.h"
#include "../Character/GTCharacter.h"
//#include "../Items/ItemWeapon.h"
#include "../Movement/NavGrid.h"
#include "Kismet/KismetMathLibrary.h"
#include "Misc/EnumRange.h"

UAction::UAction()
{
	for (EVitals vital : TEnumRange<EVitals>())
	{
		VitalCosts.Add(vital, 0);
	}
}

EActionUsable UAction::CanUseTarget(class AGTCharacter* user, FVector target)
{
	if (!user)
		return EActionUsable::Error;

	if (ANavGrid::Instance->GetDistance(target - user->GetActorLocation()) > Range + .5)
	{
		return EActionUsable::OutOfRange;
	}

	for (EVitals vital : TEnumRange<EVitals>())
	{
		//if (VitalCosts[(EVitals)i] > user->Stats->CurrentVitals[(EVitals)i])
		//	return EActionUsable::InsufficientVital;
	}

	return EActionUsable::Usable;
}

void UAction::Resolve(IActionSource source, FVector target)
{
	if (!source) return;

	UE_LOG(LogTemp, Log, TEXT("Resolving %s at %s"), *Name.ToString(), *target.ToString());
}

TArray<FVector> UAction::GetAffectedArea(IActionSource source, FVector target)
{
	TArray<FVector> result;
	result.Add(target);
	return result;
}
