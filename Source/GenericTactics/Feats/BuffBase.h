// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "BuffBase.generated.h"

/**
 * 
 */
UCLASS(ABSTRACT, EditInlineNew, BlueprintType)
class GENERICTACTICS_API UBuffBase : public UObject
{
	GENERATED_BODY()

public:
	/** In turns; does not apply to buffs from feats and equipment */
	UPROPERTY(EditInstanceOnly, Category = "Buff")
		uint8 Duration;

};
