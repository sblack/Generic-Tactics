// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "GenericTacticsGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class GENERICTACTICS_API AGenericTacticsGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
protected:
	virtual void BeginPlay() override;

public:
	AGenericTacticsGameModeBase();
	
};
