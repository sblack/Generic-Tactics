// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemEquipment.h"
#include "ItemWeapon.h"
#include "../Combat/CombatEffectDamage.h"

//#if WITH_EDITOR
//bool UItemEquipment::CanEditChange(const FProperty* InProperty) const
//{
//	const bool ParentVal = Super::CanEditChange(InProperty);
//	if (InProperty->GetFName() == FName(TEXT("Defense_Key"), FNAME_Find))
//	{
//		return false;
//	}
//	if (InProperty->GetFName() == FName(TEXT("Accuracy_Key"), FNAME_Find))
//	{
//		return false;
//	}
//	if (InProperty->GetFName() == FName(TEXT("Resist_Key"), FNAME_Find))
//	{
//		return false;
//	}
//
//	return ParentVal;
//}
//#endif
//
//void UItemEquipment::PostLoad()
//{
//	Super::PostLoad();
//}

FText FModifiedEquipment::GetName()
{
	if (Enhancement == 0)
		return BaseItem->Name;
	else
		return FText::Format(NSLOCTEXT("Items", "ModEquipName", "{0} (+{1})"), BaseItem->Name, FText::AsNumber(Enhancement));
}

TArray<class UCombatEffect*> FModifiedEquipment::GetCombatEffects()
{
	TArray<UCombatEffect*> effects;
	UItemWeapon* baseWeapon = Cast<UItemWeapon>(BaseItem);
	if (!baseWeapon)
	{
		UE_LOG(LogTemp, Error, TEXT("can't get combat effects from non-weapons"));
		return effects;
	}
	if (!baseWeapon->DamageData)
	{
		UE_LOG(LogTemp, Error, TEXT("%s is missing damage data"), *baseWeapon->Name.ToString());
		return effects;
	}

	UCombatEffectDamage* damage = NewObject<UCombatEffectDamage>();
	damage->DamageType = baseWeapon->DamageData->DamageType;
	damage->DiceCount = baseWeapon->DamageData->DiceCount;
	damage->DiceSides = baseWeapon->DamageData->DiceSides;
	damage->MinDamage = baseWeapon->DamageData->MinDamage + Enhancement;
	damage->CalcMaxDamage();
	effects.Add(damage);

	//TODO: weapon special abilities

	return effects;
}
