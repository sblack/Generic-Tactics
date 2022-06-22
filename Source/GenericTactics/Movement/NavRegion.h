// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Volume.h"
#include "NavRegion.generated.h"

UENUM(BlueprintType)
enum class ERegionTypes : uint8
{
	Void, //empty space
	None, //nothing special
	Start //starting area for player
};

/**
 * 
 */
UCLASS()
class GENERICTACTICS_API ANavRegion : public AVolume
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere)
		ERegionTypes RegionType = ERegionTypes::None;
	
};
