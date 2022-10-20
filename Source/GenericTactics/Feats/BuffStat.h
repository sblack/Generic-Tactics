// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BuffBase.h"
#include "../Utility/GTBFL.h"
#include "BuffStat.generated.h"

/**
 * Buffs that specifically modify a numeric stat
 */
UCLASS()
class GENERICTACTICS_API UBuffStat : public UBuffBase
{
	GENERATED_BODY()
public:
	/** if true, values are multiplied by recipient's level. If false, value is static */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Buff")
		bool bScaleByLevel;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Buff")
		TMap<EVitals, float> MaxVitals;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Buff")
		TMap<EAttackType, float> Defense;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Buff")
		TMap<EAttackType, float> Accuracy;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Buff")
		TMap<EDamageType, float> Resist;
};
