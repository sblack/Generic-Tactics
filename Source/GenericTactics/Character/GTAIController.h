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

	virtual void BeginTurn_Implementation();

	virtual void EndTurn_Implementation();

	const FNodeData* FindMoveData(FVector vec) const;

	virtual bool UseAI() { return true; }
public:
	UPROPERTY(BlueprintReadOnly)
		float Initiative = -1;

	UPROPERTY(BlueprintReadOnly)
		bool bIsMyTurn;

	TArray< TArray<FNodeData>> MoveGrid;
	int MoveDataID;

	UFUNCTION(BlueprintPure, Category = "Character")
		FORCEINLINE class AGTCharacter* GetTacticsCharacter();

	//virtual FPathFollowingRequestResult MoveTo(const FAIMoveRequest& MoveRequest, FNavPathSharedPtr* OutPath = nullptr) override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Combat")
		void BeginTurn();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Combat")
		void EndTurn();

	//virtual void OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result) override;

	virtual bool GetPathBack(FVector destination, TArray<FVector>& pathBack) const;

	virtual void FinishedMoving();

	//virtual FAIRequestID RequestMove(const FAIMoveRequest& MoveRequest, FNavPathSharedPtr Path) override;

	UFUNCTION(BlueprintPure, Category = "Tactics")
		TArray<FNodeData> GetReachableArea();

	void CheckAP();

	bool IsSameTeam(ITargetable target);

	UFUNCTION(BlueprintPure, Category = "Tactics")
		float CostToLocation(FVector Location);

	UFUNCTION(BlueprintPure, Category = "Tactics")
		float CostToAdjacent(FVector location, FVector& nearest);

	/** Determines furthest position pawn can reach with limit AP, moving toward destination. If limit = -1, uses remaining AP. */
	UFUNCTION(BlueprintPure, Category = "Tactics")
		FVector TruncatedPath(FVector destination, float limit = -1);
};
