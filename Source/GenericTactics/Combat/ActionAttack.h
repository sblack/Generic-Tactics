// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ActionDirect.h"
#include "../Items/ItemWeapon.h"
#include "ActionAttack.generated.h"

/**
 * Hostile action; attack roll is involved
 */
UCLASS()
class GENERICTACTICS_API UActionAttack : public UActionDirect
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Action")
		EAttackType AttackType;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Action")
		EDefenseType DefenseType;

	virtual void Resolve(TScriptInterface<IActionSourceInterface> source, FVector target) override;
};

/**
 * Attack based on the source's equipped weapon. The UActionWeapon itself serves as template; the actual 
 */
UCLASS()
class GENERICTACTICS_API UActionWeapon : public UActionAttack
{
	GENERATED_BODY()
public:
	/*UPROPERTY(EditAnywhere, BlueprintReadOnly)
		int32 FlatBonusDamage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		int32 PerDieBonusDamage;*/

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (Bitmask, BitmaskEnum = EAttackType))
		uint8 AllowedAttackTypes;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (Bitmask, BitmaskEnum = EWeaponType))
		uint8 AllowedWeaponTypes;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (Bitmask, BitmaskEnum = EHandType))
		uint8 AllowedHandTypes;

	virtual void Resolve(TScriptInterface<IActionSourceInterface> source, FVector target) override;

	bool CheckWeaponCompatible(UItemWeapon* weapon);

	UActionAttack* CreateAttack(class AHumanBase* source);

	/** basic weapon attack */
	static UActionAttack* CreateDefaultAttack(class AHumanBase* source);
};