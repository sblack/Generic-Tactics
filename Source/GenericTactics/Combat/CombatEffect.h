// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "../Utility/TargetableInterface.h"
#include "../Utility/ActionSourceInterface.h"
#include "CombatEffect.generated.h"

/**
 * 
 */
UCLASS(Abstract, EditInlineNew, BlueprintType)
class GENERICTACTICS_API UCombatEffect : public UObject
{
	GENERATED_BODY()
protected:
	static ITargetable Target;
	static IActionSource Source;

	/** Applies effect to target */
	UFUNCTION(BlueprintCallable, Category = "Combat")
		virtual void Apply() {}
	/** Applies 50% of effect to target */
	UFUNCTION(BlueprintCallable, Category = "Combat")
		virtual void ApplyHalf() { Apply(); }
	/** Applies 150% of effect to target */
	UFUNCTION(BlueprintCallable, Category = "Combat")
		virtual void ApplyCrit() { Apply(); }

public:
	/** Applies a list of effects to target */
	UFUNCTION(BlueprintCallable, Category = "Combat")
		static void ApplyEffects(TScriptInterface<ITargetableInterface> target, TArray<UCombatEffect*> effects, TScriptInterface<IActionSourceInterface> source);
	/** Applies a list of effects to target at 50% effect */
	UFUNCTION(BlueprintCallable, Category = "Combat")
		static void ApplyEffectsHalf(TScriptInterface<ITargetableInterface> target, TArray<UCombatEffect*> effects, TScriptInterface<IActionSourceInterface> source);
	/** Applies a list of effects to target at 150% effect */
	UFUNCTION(BlueprintCallable, Category = "Combat")
		static void ApplyEffectsCrit(TScriptInterface<ITargetableInterface> target, TArray<UCombatEffect*> effects, TScriptInterface<IActionSourceInterface> source);
};
