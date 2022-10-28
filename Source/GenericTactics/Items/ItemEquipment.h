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
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item|Equipment", meta = (EditCondition = "EquipType==EEquipType::Weapon || EquipType==EEquipType::Shield", EditConditionHides))
		class UPaperSprite* Sprite;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item|Equipment")
		EEquipType EquipType;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item|Equipment")
		TMap<EVitals, float> Vitals;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item|Equipment")
		TMap<EAttackType, float> Defense;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item|Equipment")
		TMap<EAttackType, float> Accuracy;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item|Equipment")
		TMap<EDamageType, float> Resist;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item|Equipment")
		uint8 Weight;

//#if WITH_EDITOR
//	virtual bool CanEditChange(const FProperty* InProperty) const override;
//#endif
//
//	virtual void PostLoad() override;
};

/**
 *
 */
 USTRUCT(BlueprintType)
struct GENERICTACTICS_API FModifiedEquipment
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UItemEquipment* BaseItem;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	uint8 Enhancement;

	//TODO: special modifiers (eg flaming, bane)

	FText GetName();

	TArray<class UCombatEffect*> GetCombatEffects();
};