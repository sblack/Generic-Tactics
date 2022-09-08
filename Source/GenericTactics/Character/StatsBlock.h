// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "../Utility/GTBFL.h"
#include "StatsBlock.generated.h"

/**
 * 
 */
UCLASS(BlueprintType)
class GENERICTACTICS_API UStatsBlock : public UObject
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadWrite)
		TMap<EVitals, int32> CurrentVitals;

	UPROPERTY(BlueprintReadWrite)
		TMap<EVitals, int32> MaxVitals;

	UPROPERTY(BlueprintReadWrite)
		TMap<EAttackType, int32> Defense;

	UPROPERTY(BlueprintReadWrite)
		TMap<EAttackType, int32> Accuracy;

	UPROPERTY(BlueprintReadWrite)
		TMap<EDamageType, int32> Resist;

	UStatsBlock(const FObjectInitializer& ObjectInitializer);

	void FillFromData(class UCharacterDataAsset* data);

	UFUNCTION(BlueprintPure)
		float GetVitalRatio(EVitals vital);
};
