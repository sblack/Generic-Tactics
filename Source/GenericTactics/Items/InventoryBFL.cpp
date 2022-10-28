// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryBFL.h"
#include "ItemEquipment.h"
#include "ItemWeapon.h"

FText UInventoryBFL::GetModEquipName(struct FModifiedEquipment ModEquip)
{
	return ModEquip.GetName();
}

FText UInventoryBFL::GetModEquipDamage(struct FModifiedEquipment ModEquip)
{
	UItemWeapon* weapon = Cast<UItemWeapon>(ModEquip.BaseItem);
	if (!weapon)
		return FText::FromString(TEXT("ERROR"));

	int min = weapon->DamageData->MinDamage + ModEquip.Enhancement;
	int max = weapon->DamageData->MaxDamage + ModEquip.Enhancement;

	return FText::Format(NSLOCTEXT("Items", "ModEquipDamage", "{0}-{1} {2}"), FText::AsNumber(min), FText::AsNumber(max), UGTBFL::DamageToText(weapon->DamageData->DamageType));
}

FText UInventoryBFL::GetModEquipDefense(struct FModifiedEquipment ModEquip, enum EAttackType AttackType)
{
	if (!ModEquip.BaseItem->Defense.Contains(AttackType) || ModEquip.BaseItem->Defense[AttackType] == 0)
		return FText::FromString(TEXT("0"));

	return FText::AsNumber(ModEquip.BaseItem->Defense[AttackType] + ModEquip.Enhancement);
}
