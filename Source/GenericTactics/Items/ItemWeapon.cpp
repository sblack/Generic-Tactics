// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemWeapon.h"

#if WITH_EDITOR
bool UItemWeapon::CanEditChange(const FProperty* InProperty) const
{
	const bool ParentVal = Super::CanEditChange(InProperty);
	if (InProperty->GetFName() == GET_MEMBER_NAME_CHECKED(UItemEquipment, EquipType))
	{
		return false;
	}

	return ParentVal;
}
#endif

void UItemWeapon::PostInitProperties()
{
	Super::PostInitProperties();

	EquipType = EEquipType::Weapon;
}

