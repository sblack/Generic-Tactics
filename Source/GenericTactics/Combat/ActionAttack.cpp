// Fill out your copyright notice in the Description page of Project Settings.


#include "ActionAttack.h"
#include "CombatManager.h"
#include "CombatEffect.h"
#include "../Character/GTCharacter.h"
#include "../Character/CharacterDataAsset.h"
#include "../Movement/NavGrid.h"

void UActionAttack::Resolve(TScriptInterface<IActionSourceInterface> source, FVector target)
{
	Super::Resolve(source, target);

	TArray<FVector> targetArea = GetAffectedArea(source, target);

	for (int i = 0; i < targetArea.Num(); i++)
	{
		FGridData data = ANavGrid::Instance->GetGridData(targetArea[i]);
		UE_LOG(LogTemp, Log, TEXT("resolve at %s"), *data.Location.ToString());
		if (data.Occupant)
		{
			UE_LOG(LogTemp, Log, TEXT("   occupant: %s"), *data.Occupant->GetDisplayName().ToString());
		}
		else
		{
			UE_LOG(LogTemp, Log, TEXT("   occupant: EMPTY"));
		}
		//TODO: team/object check
		if (data.Occupant && data.Occupant->AsActor() != source->AsActor())
		{
			switch (UCombatManager::RollAttack(this, source, data.Occupant))
			{
				case ESuccessLevel::CritHit:
					data.Occupant->OnHit(true);
					UCombatEffect::ApplyEffectsCrit(data.Occupant, Effects, source);
					break;
				case ESuccessLevel::Hit:
					data.Occupant->OnHit(false);
					UCombatEffect::ApplyEffects(data.Occupant, Effects, source);
					break;
				case ESuccessLevel::Miss:
				default:
					break;
			}
		}
	}
}

void UActionWeapon::Resolve(TScriptInterface<IActionSourceInterface> source, FVector target)
{
	UE_LOG(LogTemp, Error, TEXT("UActionWeapons should not be directly useable; instead use generated UActionAttack"));
}

bool UActionWeapon::CheckWeaponCompatible(UItemWeapon* weapon)
{
	if (!weapon) return false;

	if (!(AllowedAttackTypes & (uint8)weapon->AttackType)) return false;
	if (!(AllowedWeaponTypes & (uint8)weapon->WeaponType)) return false;
	if (!(AllowedHandTypes & (uint8)weapon->HandType)) return false;

	return true;
}

UActionAttack* UActionWeapon::CreateAttack(class AGTCharacter* source)
{
	if (!source)
	{
		UE_LOG(LogTemp, Error, TEXT("Missing source character"));
		return nullptr;
	}

	FModifiedEquipment weapon = source->CharacterData->Weapon;
	if (!weapon.BaseItem)
	{
		//TODO: default "weapon"
		UE_LOG(LogTemp, Warning, TEXT("TODO: handling no weapon equipped"));
		return nullptr;
	}

	UItemWeapon* baseWeapon = Cast<UItemWeapon>(weapon.BaseItem);

	UActionAttack* attack = NewObject<UActionAttack>();
	//if Name is empty, this is a default attack; use weapon's name instead
	attack->Name = (Name.IsEmptyOrWhitespace()) ? weapon.GetName() : Name;
	attack->Range = FMath::Max(baseWeapon->Range, Range);
	attack->Anim = (Anim == EActionAnim::Attack) ? baseWeapon->Anim : Anim;
	attack->ProjectileClass = (ProjectileClass == nullptr) ? baseWeapon->ProjectileClass : ProjectileClass;
	attack->Particles.Append(baseWeapon->Particles);
	attack->Particles.Append(Particles); //entries in Particles (should) override entries with matching keys
	attack->Sounds.Append(baseWeapon->Sounds);
	attack->Sounds.Append(Sounds); //entries in Sounds (should) override entries with matching keys


	attack->Effects = weapon.GetCombatEffects();
	attack->Effects.Append(Effects);


	attack->AttackType = baseWeapon->AttackType;
	attack->APCost = APCost;
	attack->VitalCosts = VitalCosts;
	attack->Particles = Particles;
	attack->Sounds = Sounds;
	attack->Area = Area;

	return attack;
}

UActionAttack* UActionWeapon::CreateDefaultAttack(class AGTCharacter* source)
{
	if (!source)
	{
		UE_LOG(LogTemp, Error, TEXT("Missing source character"));
		return nullptr;
	}

	FModifiedEquipment weapon = source->CharacterData->Weapon;
	if (!weapon.BaseItem)
	{
		//TODO: default "weapon"
		UE_LOG(LogTemp, Warning, TEXT("TODO: handling no weapon equipped"));
		return nullptr;
	}

	UItemWeapon* baseWeapon = Cast<UItemWeapon>(weapon.BaseItem);

	UActionAttack* attack = NewObject<UActionAttack>();
	//if Name is empty, this is a default attack; use weapon's name instead
	attack->Name = weapon.GetName();
	attack->Range = baseWeapon->Range;
	attack->Anim = baseWeapon->Anim;
	attack->ProjectileClass = baseWeapon->ProjectileClass;
	attack->Particles.Append(baseWeapon->Particles);
	attack->Sounds.Append(baseWeapon->Sounds);

	attack->Effects = weapon.GetCombatEffects();

	attack->AttackType = baseWeapon->AttackType;
	attack->APCost = 1;

	return attack;
}
