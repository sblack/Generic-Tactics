// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "../Utility/GTBFL.h"
#include "../Utility/TargetableInterface.h"
#include "Action.generated.h"

UENUM(BlueprintType)
enum class EActionUsable : uint8
{
	Usable,
	OutOfRange,
	InsufficientAP,
	InsufficientVital,
	Error
};

/**
 * 
 */
UCLASS(ABSTRACT, BlueprintType, Const, EditInlineNew)
class GENERICTACTICS_API UAction : public UObject
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		FText Name;

	/** True range is +.5, for "rounding" */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		float Range;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		float APCost;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		TMap<EVitals, int32> VitalCosts;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		EActionAnim Anim;

	UAction();

	UFUNCTION(BlueprintPure, Category = "Action")
		virtual EActionUsable CanUseTarget(class AGTCharacter* user, FVector target);
	
	virtual void Perform(class AGTCharacter* user, FVector target);

	virtual void Resolve(class AGTCharacter* user, FVector target);

	virtual TArray<FVector> GetAffectedArea(class AGTCharacter* source, FVector target);
};
