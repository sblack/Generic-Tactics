// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "../Movement/NavGrid.h"
#include "GTAIController.generated.h"

/**
 * 
 */
UCLASS(meta = (DisplayName = "GT AI Controller"))
class GENERICTACTICS_API AGTAIController : public AAIController
{
	GENERATED_BODY()
protected:

	virtual void BeginPlay() override;
public:

	UFUNCTION(BlueprintPure, Category = "Character")
		FORCEINLINE class AGTCharacter* GetTacticsCharacter();
};
