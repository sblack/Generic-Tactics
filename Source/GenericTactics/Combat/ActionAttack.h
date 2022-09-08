// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ActionDirect.h"
#include "ActionAttack.generated.h"

/**
 * 
 */
UCLASS()
class GENERICTACTICS_API UActionAttack : public UActionDirect
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		EAttackType AttackType;

	virtual void Resolve(class AGTCharacter* user, FVector target) override;
};
