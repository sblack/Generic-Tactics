// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyDataAsset.h"
#include "../Feats/Feat.h"

uint8 UEnemyDataAsset::GetLevel()
{
	TotalXP = 0;
	for (UFeat* feat : Feats)
		TotalXP += feat->XPCost;

	uint8 level = Super::GetLevel();
	if (ForceLevel > level)
		level = ForceLevel;

	return level;
}
