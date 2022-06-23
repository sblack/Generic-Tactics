// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "AIObjective.generated.h"

/**
 * Individual goals that AI attempts according to priority.
 */
UCLASS(ABSTRACT, Blueprintable)
class GENERICTACTICS_API UAIObjective : public UObject
{
	GENERATED_BODY()
public:

	UFUNCTION(BlueprintImplementableEvent)
		bool Attempt(class AGTCharacter* character);

	UFUNCTION(BlueprintImplementableEvent, meta = (DevelopmentOnly))
		FString GetDebugString();
};
