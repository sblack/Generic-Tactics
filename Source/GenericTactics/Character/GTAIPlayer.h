// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GTAIController.h"
#include "GTAIPlayer.generated.h"

/**
 * 
 */
UCLASS(meta = (DisplayName = "GT AI Player"))
class GENERICTACTICS_API AGTAIPlayer : public AGTAIController
{
	GENERATED_BODY()
protected:
	virtual void BeginTurn_Implementation() override;

	virtual void EndTurn_Implementation() override;

	virtual bool UseAI() override { return false; }

public:
	//virtual void OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result) override;

	virtual bool GetPathBack(FVector destination, TArray<FVector>& pathBack) const override;
};
