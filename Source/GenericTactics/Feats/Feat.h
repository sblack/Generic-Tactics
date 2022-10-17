// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Feat.generated.h"

/**
 * 
 */
UCLASS(Const, BlueprintType)
class GENERICTACTICS_API UFeat : public UDataAsset
{
	GENERATED_BODY()
public:
	UPROPERTY(EditDefaultsOnly)
		FText Name;

	UPROPERTY(EditDefaultsOnly)
		uint8 XPCost = 10;

	UPROPERTY(EditDefaultsOnly)
		TArray<UFeat*> PreReqs;

	UPROPERTY(EditDefaultsOnly)
		uint8 MinLevel = 0;

	UPROPERTY(EditDefaultsOnly, Instanced)
		TArray<class UAction*> Actions;

	UPROPERTY(EditDefaultsOnly, Instanced)
		TArray<class UBuffBase*> Passives;
};
