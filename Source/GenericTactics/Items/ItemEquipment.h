// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item.h"
#include "../Utility/GTBFL.h"
#include "ItemEquipment.generated.h"

UENUM(BlueprintType)
enum class EEquipType : uint8
{
	Weapon,
	Shield,
	Armor,
	Accessory
};

/**
 * 
 */
UCLASS()
class GENERICTACTICS_API UItemEquipment : public UItem
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		EEquipType EquipType;

	UPROPERTY(EditAnywhere, EditFixedSize, BlueprintReadOnly)
		TMap<EAttackType, float> Defense;

	UPROPERTY(EditAnywhere, EditFixedSize, BlueprintReadOnly)
		TMap<EAttackType, float> Accuracy;

	UPROPERTY(EditAnywhere, EditFixedSize, BlueprintReadOnly)
		TMap<EDamageType, float> Resist;

	UPROPERTY(EditAnywhere, EditFixedSize, BlueprintReadOnly)
		TMap<EDamageType, float> Penetrate;

#if WITH_EDITOR
	virtual bool CanEditChange(const FProperty* InProperty) const override;
#endif

	virtual void PostLoad() override;
};
