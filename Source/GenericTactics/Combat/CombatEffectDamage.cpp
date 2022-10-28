// Fill out your copyright notice in the Description page of Project Settings.


#include "CombatEffectDamage.h"
#include "CombatManager.h"

void UCombatEffectDamage::Apply()
{
	int amount = MinDamage;
	for (int i = 0 ; i < DiceCount; i++)
		amount += FMath::RandRange(0, DiceSides - 1);

	if (amount < 0) amount = 0; //never do negative damage

	amount = Target->ApplyDamage(amount, DamageType, TargetVital);
	UCombatManager::CombatManager()->SpawnDamageIndicator(Target->GetLocation(), amount, ESuccessLevel::Hit);
}

void UCombatEffectDamage::ApplyHalf()
{
	int amount = MinDamage;
	for (int i = 0; i < DiceCount; i++)
		amount += FMath::RandRange(0, DiceSides - 1);

	amount /= 2;
	if (amount < 0) amount = 0; //never do negative damage

	amount = Target->ApplyDamage(amount, DamageType, TargetVital);
	UCombatManager::CombatManager()->SpawnDamageIndicator(Target->GetLocation(), amount, ESuccessLevel::Hit); //TODO
}

void UCombatEffectDamage::ApplyCrit()
{
	int amount = MinDamage;
	for (int i = 0; i < DiceCount; i++)
		amount += FMath::RandRange(0, DiceSides - 1);

	amount *= 1.5;
	if (amount < MaxDamage) amount = MaxDamage; //crits don't do less than best non-crit
	if (amount < 0) amount = 0; //never do negative damage

	amount = Target->ApplyDamage(amount, DamageType, TargetVital);
	UCombatManager::CombatManager()->SpawnDamageIndicator(Target->GetLocation(), amount, ESuccessLevel::CritHit);
}

void UCombatEffectDamage::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	FName PropertyName = (PropertyChangedEvent.Property != NULL) ? PropertyChangedEvent.Property->GetFName() : NAME_None;
	if (PropertyName == GET_MEMBER_NAME_CHECKED(UCombatEffectDamage, MinDamage) ||
		PropertyName == GET_MEMBER_NAME_CHECKED(UCombatEffectDamage, DiceSides) ||
		PropertyName == GET_MEMBER_NAME_CHECKED(UCombatEffectDamage, DiceCount))
	{
		CalcMaxDamage();
	}
}

void UCombatEffectDamage::CalcMaxDamage()
{
	MaxDamage = MinDamage + DiceCount * (DiceSides - 1);
}
