// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "../Utility/GTBFL.h"
#include "../Utility/ActionSourceInterface.h"
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

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		class UTexture2D* Icon;

	/** True range is +.5, for "rounding" */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Targeting")
		float Range;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Cost", meta = (ClampMin = 1, ClampMax = 3))
		uint8 ActionCost = 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Cost")
		TMap<EVitals, int32> VitalCosts;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FX")
		EActionAnim Anim;

	/** timing and location determined by key */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FX")
		TMap<FName, class UParticleSystem*> Particles;

	/** timing and location determined by key */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FX")
		TMap<FName, class USoundBase*> Sounds;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FX")
		TSubclassOf<class AProjectile> ProjectileClass;

	UAction();

	UFUNCTION(BlueprintPure, Category = "Action", meta = (DeprecatedFunction))
		virtual EActionUsable CanUseTarget(class AGTCharacter* user, FVector target);

	virtual void Resolve(IActionSource source, FVector target);

	virtual TArray<FVector> GetAffectedArea(IActionSource source, FVector target);
};
