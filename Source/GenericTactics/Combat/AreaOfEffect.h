// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "../Utility/ActionSourceInterface.h"
#include "AreaOfEffect.generated.h"

/**
 * 
 */
UCLASS(ABSTRACT, Blueprintable, EditInlineNew)
class GENERICTACTICS_API UAreaOfEffect : public UObject
{
	GENERATED_BODY()
public:
	/** Gets all squares in AoE, */
	UFUNCTION(BlueprintImplementableEvent)
		TArray<FVector> GetTargetedSquares(const TScriptInterface<IActionSourceInterface>& source, FVector center);
};
