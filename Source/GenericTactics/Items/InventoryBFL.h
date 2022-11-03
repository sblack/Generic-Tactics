// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "InventoryBFL.generated.h"

/**
 * 
 */
UCLASS()
class GENERICTACTICS_API UInventoryBFL : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:

	UFUNCTION(BlueprintPure)
		static FText GetModEquipName(struct FModifiedEquipment ModEquip);

	UFUNCTION(BlueprintPure)
		static FText GetModEquipDamage(struct FModifiedEquipment ModEquip);

	UFUNCTION(BlueprintPure)
		static FText GetModEquipDefense(struct FModifiedEquipment ModEquip, enum EDefenseType DefenseType);
};
