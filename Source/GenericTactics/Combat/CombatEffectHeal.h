// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CombatEffect.h"
#include "CombatEffectHeal.generated.h"

/**
 * 
 */
UCLASS()
class GENERICTACTICS_API UCombatEffectHeal : public UCombatEffect
{
	GENERATED_BODY()
protected:
	/** Applies effect to target */
	virtual void Apply() override;

public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		EVitals TargetVital = EVitals::Health;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		uint8 Amount;
};
