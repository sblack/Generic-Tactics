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
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		FText Name;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		uint8 XPCost = 10;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		TArray<UFeat*> PreReqs;

	/** Feats that require this feat as a prereq*/
	UPROPERTY(Transient, BlueprintReadOnly)
		TArray<UFeat*> DerivedFeats;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		uint8 MinLevel = 0;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		FText Description;

	/** characters are limited on chassis feats, to discourage just pumping up stats as quickly as possible */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		bool bChassisFeat;

	UPROPERTY(EditDefaultsOnly, Instanced, BlueprintReadOnly)
		TArray<class UAction*> Actions;

	UPROPERTY(EditDefaultsOnly, Instanced, BlueprintReadOnly)
		TArray<class UBuffBase*> Passives;

	UFUNCTION(BlueprintPure)
	FText GetReqText();
};
