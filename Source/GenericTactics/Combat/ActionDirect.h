// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Action.h"
#include "ActionDirect.generated.h"

/**
 * 
 */
UCLASS()
class GENERICTACTICS_API UActionDirect : public UAction
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Instanced)
		class UAreaOfEffect* Area;

	TArray<FVector> GetAffectedArea(class AGTCharacter* source, FVector target) override;

};
