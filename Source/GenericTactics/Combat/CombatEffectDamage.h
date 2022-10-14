// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CombatEffect.h"
#include "../Utility/GTBFL.h"
#include "CombatEffectDamage.generated.h"

/**
 * 
 */
UCLASS()
class GENERICTACTICS_API UCombatEffectDamage : public UCombatEffect
{
	GENERATED_BODY()
protected:
	/** Applies effect to target */
	virtual void Apply() override;
	/** Applies 50% of effect to target */
	virtual void ApplyHalf() override;
	/** Applies 150% of effect to target */
	virtual void ApplyCrit() override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		EDamageType DamageType;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		EVitals TargetVital = EVitals::Health;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		uint8 DiceCount;

	/** dice are 0 - (N-1), not 1 - N */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ClampMin = 2))
		uint8 DiceSides = 2;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		int32 MinDamage;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		int32 MaxDamage;

#if WITH_EDITOR
	void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

};
