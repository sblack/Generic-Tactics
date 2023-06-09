// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "AIObjective.generated.h"

/**
 * Individual goals that AI attempts according to priority.
 */
UCLASS(ABSTRACT, Blueprintable, EditInlineNew)
class GENERICTACTICS_API UAIObjective : public UObject
{
	GENERATED_BODY()
protected:
	UFUNCTION(BlueprintCallable)
		TArray<class AGTCharacter*> GetAllEnemiesInRange(class AGTCharacter* character, float range);

public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		FString DebugString = "ERROR";

	UFUNCTION(BlueprintImplementableEvent)
		bool Attempt(class AGTCharacter* character);
};
