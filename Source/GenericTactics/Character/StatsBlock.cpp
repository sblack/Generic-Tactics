// Fill out your copyright notice in the Description page of Project Settings.


#include "StatsBlock.h"
#include "CharacterDataAsset.h"
#include "../Feats/Feat.h"
#include "../Feats/BuffBase.h"
#include "../Feats/BuffStat.h"
#include "Misc/EnumRange.h"

UStatsBlock::UStatsBlock(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	for (EVitals vit : TEnumRange<EVitals>())
	{
		CurrentVitals.Add(vit, 0);
		MaxVitals.Add(vit, 0);
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
	Level = data->Level;
	{
		TMap<EVitals, float> scaleVital;
		for (EVitals vit : TEnumRange<EVitals>())
		{
			scaleVital.Add(vit, 0);
		}
		scaleVital[EVitals::Health] = 6;
		TMap<EAttackType, float> scaleDefense, scaleAccuracy;
		for (EAttackType att : TEnumRange<EAttackType>())
		{
			scaleDefense.Add(att, 0);
			scaleAccuracy.Add(att, 0);
		}
		TMap<EDamageType, float> scaleResist;
		for (EDamageType dam : TEnumRange<EDamageType>())
		{
			scaleResist.Add(dam, 0);
		}

		for (UFeat* feat : data->Feats)
		{
			for (UBuffBase* buff : feat->Passives)
			{
				UBuffStat* buffStat = Cast<UBuffStat>(buff);
				if(!buffStat) continue;

				for (auto& elem : buffStat->MaxVitals)
				{
					if(buffStat->bScaleByLevel) scaleVital[elem.Key] += elem.Value;
					else MaxVitals[elem.Key] += elem.Value;
				}

				for (auto& elem : buffStat->Defense)
				{
					if (buffStat->bScaleByLevel) scaleDefense[elem.Key] += elem.Value;
					else Defense[elem.Key] += elem.Value;
				}

				for (auto& elem : buffStat->Accuracy)
				{
					if (buffStat->bScaleByLevel) scaleAccuracy[elem.Key] += elem.Value;
					else Accuracy[elem.Key] += elem.Value;
				}

				for (auto& elem : buffStat->Resist)
				{
					if (buffStat->bScaleByLevel) scaleResist[elem.Key] += elem.Value;
					else Resist[elem.Key] += elem.Value;
				}

				//TODO: handle non-stat buffs
			}
		}
		//TODO: buffs from equipment


		for (EVitals vit : TEnumRange<EVitals>())
		{
			MaxVitals[vit] += (int)(scaleVital[vit] * Level);
		}
		for (EAttackType att : TEnumRange<EAttackType>())
		{
			Defense[att] += (int)(scaleDefense[att] * Level);
			Accuracy[att] += (int)(scaleAccuracy[att] * Level);
		}
		for (EDamageType dam : TEnumRange<EDamageType>())
		{
			Resist[dam] += (int)(scaleResist[dam] * Level);
		}
	}

	//health and stamina start at full, mana starts empty
	CurrentVitals[EVitals::Health] = MaxVitals[EVitals::Health];
	CurrentVitals[EVitals::Mana] = 0;
	CurrentVitals[EVitals::Stamina] = MaxVitals[EVitals::Stamina];
}

float UStatsBlock::GetVitalRatio(EVitals vital)
{
	if (MaxVitals[vital] == 0) return 0;

	return (float)CurrentVitals[vital] / (float)MaxVitals[vital];
}

FString UStatsBlock::GetVitalRatioString(EVitals vital)
{
	return FString::Printf(TEXT("%d / %d"), CurrentVitals[vital], MaxVitals[vital]);
}
