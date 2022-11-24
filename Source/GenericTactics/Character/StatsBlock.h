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
private:
	void CalcStats(class UCharacterDataAsset* data);
public:
	UPROPERTY(BlueprintReadWrite)
		uint8 Level;

	UPROPERTY(BlueprintReadWrite)
		TMap<EVitals, int32> CurrentVitals;

	UPROPERTY(BlueprintReadWrite)
		TMap<EVitals, int32> MaxVitals;

	UPROPERTY(BlueprintReadWrite)
		TMap<ECharStat, int32> Stats;

	UPROPERTY(BlueprintReadWrite)
		TMap<EDefenseType, int32> Defense;

	UPROPERTY(BlueprintReadWrite)
		TMap<EAttackType, int32> Accuracy;

	UPROPERTY(BlueprintReadWrite)
		TMap<EDamageType, int32> Resist;

	UPROPERTY(BlueprintReadWrite)
		uint8 MaxActions = 3;

	UPROPERTY(BlueprintReadWrite)
		uint8 MoveSpeed = 5;

	UPROPERTY(BlueprintReadOnly)
	int32 DetectionRadius = 2;

	UPROPERTY(BlueprintReadOnly)
	int32 SpellLevel;

	// not including dex modifier
	UPROPERTY(BlueprintReadOnly)
	int32 InitiativeBonus = 0;

	UStatsBlock(const FObjectInitializer& ObjectInitializer);

	UFUNCTION(BlueprintCallable)
	void Reset();

	UFUNCTION(BlueprintCallable)
	void FillFromData(class UCharacterDataAsset* data);

	UFUNCTION(BlueprintPure)
		float GetVitalRatio(EVitals vital);

	UFUNCTION(BlueprintPure)
		FString GetVitalRatioString(EVitals vital);
};
