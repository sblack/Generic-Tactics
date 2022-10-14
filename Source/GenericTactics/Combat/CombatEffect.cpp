// Fill out your copyright notice in the Description page of Project Settings.


#include "CombatEffect.h"

ITargetable UCombatEffect::Target;
IActionSource UCombatEffect::Source;

void UCombatEffect::ApplyEffects(TScriptInterface<ITargetableInterface> target, TArray<UCombatEffect*> effects, TScriptInterface<IActionSourceInterface> source)
{
	if (target == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("Missing target"));
		UE_LOG(LogTemp, Log, TEXT("AE: %s"), (source != nullptr) ? (*source->GetDisplayName().ToString()) : (TEXT("NULL SOURCE")));
		return;
	}

	Target = target;
	Source = source;

	for (int i = 0; i < effects.Num(); i++)
	{
		effects[i]->Apply();
	}
}

void UCombatEffect::ApplyEffectsHalf(TScriptInterface<ITargetableInterface> target, TArray<UCombatEffect*> effects, TScriptInterface<IActionSourceInterface> source)
{
	if (target == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("Missing target"));
		UE_LOG(LogTemp, Log, TEXT("AEH: %s"), (source != nullptr) ? (*source->GetDisplayName().ToString()) : (TEXT("NULL SOURCE")));
		return;
	}

	Target = target;
	Source = source;

	for (int i = 0; i < effects.Num(); i++)
	{
		effects[i]->ApplyHalf();
	}
}

void UCombatEffect::ApplyEffectsCrit(TScriptInterface<ITargetableInterface> target, TArray<UCombatEffect*> effects, TScriptInterface<IActionSourceInterface> source)
{
	if (target == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("Missing target"));
		UE_LOG(LogTemp, Log, TEXT("AEC: %s"), (source != nullptr) ? (*source->GetDisplayName().ToString()) : (TEXT("NULL SOURCE")));
		return;
	}

	Target = target;
	Source = source;

	for (int i = 0; i < effects.Num(); i++)
	{
		effects[i]->ApplyCrit();
	}
}
