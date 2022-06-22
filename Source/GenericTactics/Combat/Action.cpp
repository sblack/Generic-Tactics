// Fill out your copyright notice in the Description page of Project Settings.


#include "Action.h"
#include "CombatManager.h"
#include "../Character/GTCharacter.h"
#include "../Items/ItemWeapon.h"
#include "Kismet/KismetMathLibrary.h"

UAction::UAction()
{
	for (int i = 0 ; i < (int)EVitals::COUNT; i++)
	{
		VitalCosts.Add((EVitals)i, 0);
	}
}

EActionUsable UAction::CanUseTarget(class AGTCharacter* user, TScriptInterface<ITargetableInterface> target)
{
	if (!user || !target)
		return EActionUsable::Error;

	for (int i = 0; i < (int)EVitals::COUNT; i++)
	{
		//if (VitalCosts[(EVitals)i] > user->Stats->CurrentVitals[(EVitals)i])
		//	return EActionUsable::InsufficientVital;
	}

	return EActionUsable::Usable;
}

void UAction::Perform(class AGTCharacter* user, FVector direction)
{
	if (!direction.IsZero())
	{
		user->SetActorRotation(UKismetMathLibrary::MakeRotFromX(direction));
	}

	if (GetOuter()->IsA<UItemWeapon>())
	{
		//user->SetWeaponSprite((Cast<UItemWeapon>(GetOuter()))->Sprite);
	}

	user->ActionInProgress.Action = this;
	user->ActionInProgress.RelativeLocation = direction;
	//user->PlayActionAnim(Anim);
	user->CurrentAP -= APCost;

	for (int i = 0; i < (int)EVitals::COUNT; i++)
	{
		//if (VitalCosts[(EVitals)i] > 0)
		//	user->Stats->CurrentVitals[(EVitals)i] -= VitalCosts[(EVitals)i];
	}
}

void UAction::Resolve(class AGTCharacter* user, FVector direction)
{
	UE_LOG(LogTemp, Log, TEXT("Resolve"));
}