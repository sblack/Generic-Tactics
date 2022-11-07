// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GTCharacter.h"
#include "../Combat/CombatManager.h"
#include "../Movement/NavGrid.h"
#include "../Utility/GTBFL.h"
#include "MonsterBase.generated.h"

/**
 * 
 */
UCLASS(Abstract)
class GENERICTACTICS_API AMonsterBase : public AGTCharacter
{
	GENERATED_BODY()
	//SPRITES AND COSMETIC
private:
	virtual void InitMaterials() override;

public:
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Sprite")
		FLinearColor Color0;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Sprite")
		FLinearColor Color1;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Sprite")
		FLinearColor Color2;

	/** used for UI */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Sprite")
		class UTexture2D* Image;

	UPROPERTY(Transient, BlueprintReadOnly)
		class UMaterialInstanceDynamic* UIDMI;


protected:
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Stats")
		TMap<EVitals, int32> MaxVitals;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Stats")
		TMap<EDefenseType, int32> Defense;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Stats")
		TMap<EAttackType, int32> Accuracy;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Stats")
		TMap<EDamageType, int32> Resist;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Stats")
		int32 DetectionRadius = 2;

	virtual void SetStats() override;



protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Instanced, Category = "AI")
		TArray<class UAIObjective*> AIObjectives;

	virtual void AdvanceAI() override;
};
