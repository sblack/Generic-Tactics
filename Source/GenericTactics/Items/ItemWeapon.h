// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ItemEquipment.h"
#include "ItemWeapon.generated.h"

UENUM(BlueprintType, meta = (Bitflags, UseEnumValuesAsMaskValuesInEditor = "true"))
enum class EHandType : uint8
{
	None = 0 UMETA(Hidden),
	One = 1 << 0,
	Mix = 1 << 1,
	Two = 1 << 2,
};
ENUM_CLASS_FLAGS(EHandType);

UENUM(BlueprintType, meta = (Bitflags, UseEnumValuesAsMaskValuesInEditor = "true"))
enum class EWeaponType : uint8
{
	None = 0 UMETA(Hidden),
	Axe = 1 << 0,
	Bow = 1 << 1,
	Fist = 1 << 2,
	Spear = 1 << 3,
	Hammer = 1 << 4,
	LightBlade = 1 << 5,
	HeavyBlade = 1 << 6,
	Staff = 1 << 7,
};
ENUM_CLASS_FLAGS(EWeaponType);


/**
 * 
 */
UCLASS()
class GENERICTACTICS_API UItemWeapon : public UItemEquipment
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item|Weapon")
		EHandType HandType;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item|Weapon")
		EWeaponType WeaponType;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item|Weapon")
		EAttackType AttackType;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item|Weapon")
		EDefenseType DefenseType;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item|Weapon")
		EActionAnim Anim;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Instanced, Category = "Item|Weapon")
		class UCombatEffectDamage* DamageData;

	/** Also serves as reach; true range is +.5, for "rounding"*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item|Weapon", meta = (ClampMin = 1, Multiple = 0.5f))
		float Range = 1;

	/** timing and location determined by key */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item|Weapon")
		TMap<FName, class UParticleSystem*> Particles;

	/** timing and location determined by key */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item|Weapon")
		TMap<FName, class USoundBase*> Sounds;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item|Weapon")
		TSubclassOf<class AProjectile> ProjectileClass;

	// Note the CanEditChange() function is only available when compiling with the editor.
// Make sure to wrap it with the WITH_EDITOR define or your builds fail!
#if WITH_EDITOR
	virtual bool CanEditChange(const FProperty* InProperty) const override;
#endif

	virtual void PostInitProperties() override;
};