// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Action.h"
#include "ActionMove.generated.h"

/**
 * 
 */
UCLASS(NotEditInlineNew)
class GENERICTACTICS_API UActionMove : public UAction
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadOnly, meta = (ExposeOnSpawn = true))
		FVector Destination;

	//void Perform(class AGTCharacter* user, FVector direction) override;

	//void Resolve(class AGTCharacter* user, FVector direction) override;

};
