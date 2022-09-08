// Fill out your copyright notice in the Description page of Project Settings.


#include "StatsBlock.h"
#include "CharacterDataAsset.h"
#include "Misc/EnumRange.h"

UStatsBlock::UStatsBlock(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	for (EVitals vit : TEnumRange<EVitals>())
	{
		CurrentVitals.Add(vit, 0);
		MaxVitals.Add(vit, 10);
	}

	for (EAttackType att : TEnumRange<EAttackType>())
	{
		Defense.Add(att, 0);
		Accuracy.Add(att, 0);
	}

	for (EDamageType dam : TEnumRange<EDamageType>())
	{
		Resist.Add(dam, 0);
	}
}

void UStatsBlock::FillFromData(class UCharacterDataAsset* data)
{
	//health and stamina start at full, mana starts empty
	CurrentVitals[EVitals::Health] = MaxVitals[EVitals::Health];
	CurrentVitals[EVitals::Mana] = 0;
	CurrentVitals[EVitals::Stamina] = MaxVitals[EVitals::Stamina];
}

float UStatsBlock::GetVitalRatio(EVitals vital)
{
	return (float)CurrentVitals[vital] / (float)MaxVitals[vital];
}
