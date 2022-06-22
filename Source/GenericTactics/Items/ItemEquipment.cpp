// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemEquipment.h"

#if WITH_EDITOR
bool UItemEquipment::CanEditChange(const FProperty* InProperty) const
{
	const bool ParentVal = Super::CanEditChange(InProperty);
	if (InProperty->GetFName() == FName(TEXT("Defense_Key"), FNAME_Find))
	{
		return false;
	}
	if (InProperty->GetFName() == FName(TEXT("Accuracy_Key"), FNAME_Find))
	{
		return false;
	}
	if (InProperty->GetFName() == FName(TEXT("Resist_Key"), FNAME_Find))
	{
		return false;
	}
	if (InProperty->GetFName() == FName(TEXT("Penetrate_Key"), FNAME_Find))
	{
		return false;
	}

	return ParentVal;
}
#endif

void UItemEquipment::PostLoad()
{
	Super::PostLoad();

	for (int i = 0; i < 3; i++)
	{
		if (!Defense.Contains((EAttackType)i))
		{
			Defense.Add((EAttackType)i, 0);
		}
		if (!Accuracy.Contains((EAttackType)i))
		{
			Accuracy.Add((EAttackType)i, 0);
		}
	}

	for (int i = 0; i < (int)EDamageType::Typeless; i++)
	{
		if (!Resist.Contains((EDamageType)i))
		{
			Resist.Add((EDamageType)i, 0);
		}
		if (!Penetrate.Contains((EDamageType)i))
		{
			Penetrate.Add((EDamageType)i, 0);
		}
	}
}