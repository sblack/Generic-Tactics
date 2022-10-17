// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CharacterDataAsset.h"
#include "EnemyDataAsset.generated.h"

/**
 * Template to be used by enemies (and really any non-party member)
 */
UCLASS()
class GENERICTACTICS_API UEnemyDataAsset : public UCharacterDataAsset
{
	GENERATED_BODY()
public:
	/** if greater than level calculated from feats, raises level to this */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stats")
		uint8 ForceLevel = 0;

	uint8 GetLevel() override;

};
