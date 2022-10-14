// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CombatEffect.h"
#include "CombatEffectBuff.generated.h"

/**
 * 
 */
UCLASS()
class GENERICTACTICS_API UCombatEffectBuff : public UCombatEffect
{
	GENERATED_BODY()
protected:
	/** Applies effect to target */
	//virtual void Apply() override;

	/*virtual void ApplyCrit() override;

	virtual void ApplyHalf() override;*/

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Instanced)
		TArray<class UBuffBase*> Buffs;

	//void ApplyBuffs(TScriptInterface<ITargetableInterface> target);
};
