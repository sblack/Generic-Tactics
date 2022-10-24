// Fill out your copyright notice in the Description page of Project Settings.


#include "StatsBlock.h"
#include "CharacterDataAsset.h"
#include "../Feats/Feat.h"
#include "../Feats/BuffBase.h"
#include "../Feats/BuffStat.h"
#include "Misc/EnumRange.h"

void UStatsBlock::CalcStats(class UCharacterDataAsset* data)
{
	for (ECharStat sta : TEnumRange<ECharStat>())
	{
		Stats[sta] = 0;
	}

	for (uint8 choice : data->StatChoices)
	{
		if (choice & 1) Stats[ECharStat::Str] += 1;
		if (choice & 2) Stats[ECharStat::Dex] += 1;
		if (choice & 4) Stats[ECharStat::Con] += 1;
		if (choice & 8) Stats[ECharStat::Int] += 1;
		if (choice & 16) Stats[ECharStat::Wis] += 1;
	}
}

UStatsBlock::UStatsBlock(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	for (EVitals vit : TEnumRange<EVitals>())
	{
		CurrentVitals.Add(vit, 0);
		MaxVitals.Add(vit, 0);
	}

	for (ECharStat sta : TEnumRange<ECharStat>())
	{
		Stats.Add(sta, 0);
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

void UStatsBlock::Reset()
{
	for (EVitals vit : TEnumRange<EVitals>())
	{
		CurrentVitals[vit] = 0;
		MaxVitals[vit] = 0;
	}

	for (ECharStat sta : TEnumRange<ECharStat>())
	{
		Stats[sta] = 0;
	}

	for (EAttackType att : TEnumRange<EAttackType>())
	{
		Defense[att] = 0;
		Accuracy[att] = 0;
	}

	for (EDamageType dam : TEnumRange<EDamageType>())
	{
		Resist[dam] = 0;
	}
}

void UStatsBlock::FillFromData(class UCharacterDataAsset* data)
{
	Level = data->GetLevel();
	//characters should start with enough XP to be level 1, but force it
	if (Level < 1) Level = 1;
	SpellLevel = (int)((Level + 1) / 2);

	CalcStats(data);

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

				//no feats that boost base stats?

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

		//special handling of Vitals
		{
			scaleVital[EVitals::Health] += Stats[ECharStat::Con];
			MaxVitals[EVitals::Health] += (int)(scaleVital[EVitals::Health] * Level);

			//if character doesn't have basic mana feat; no mana regardless of other sources
			if (scaleVital[EVitals::Mana] == 0) 
				MaxVitals[EVitals::Mana] = 0;
			else
			{
				MaxVitals[EVitals::Mana] += (int)(scaleVital[EVitals::Mana] * Level);
				//... otherwise, guaranteed at least one mana from level
				if (MaxVitals[EVitals::Mana] < 1)
					MaxVitals[EVitals::Mana] = 1;

				MaxVitals[EVitals::Mana] += FMath::Min(SpellLevel, FMath::Max(Stats[ECharStat::Int], Stats[ECharStat::Wis]));
			}
			
			//same for stamina
			if (scaleVital[EVitals::Stamina] == 0)
				MaxVitals[EVitals::Stamina] = 0;
			else
			{
				MaxVitals[EVitals::Stamina] += (int)(scaleVital[EVitals::Stamina] * Level);
				if (MaxVitals[EVitals::Stamina] < 1)
					MaxVitals[EVitals::Stamina] = 1;

				MaxVitals[EVitals::Stamina] += (int)(Stats[ECharStat::Con] * (.8 + Level * .2));
			}
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
