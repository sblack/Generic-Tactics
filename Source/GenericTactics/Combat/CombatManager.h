// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "../Utility/TargetableInterface.h"
#include "CombatManager.generated.h"

/**
 *
 */
USTRUCT(BlueprintType)
struct FActionData
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(BlueprintReadWrite)
		class UAction* Action;

	UPROPERTY(BlueprintReadWrite)
		class AGTCharacter* Actor;

	UPROPERTY(BlueprintReadWrite)
		FVector Location;

};


/**
 * 
 */
UCLASS(ABSTRACT, Blueprintable)
class GENERICTACTICS_API UCombatManager : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
protected:
	static UCombatManager* Instance;

	UPROPERTY(BlueprintReadWrite)
		TArray<class AGTCharacter*> InitiativeQueue;

	UPROPERTY(EditDefaultsOnly, Instanced)
		class UActionAttack* DefaultAttack;

	int InitiativeIndex;

	bool bPerformingActions = false;

	//TArray<FActionData> InterruptQueue;
	//TArray<FActionData> ReactionQueue;

	TArray<FActionData> ActionQueue;

	//bool bInterrupting = false;

	bool bPreCombat = true;

	//FTimerHandle InterruptHandle;

	//enemies of acting character, not of player
	TArray<class AGTCharacter*> AwareEnemies;

	//DO I STILL NEED THIS?
	void AdvanceActionQueue();

	/** True = hit */
	UFUNCTION(BlueprintImplementableEvent, Category = "Combat")
		bool AttackRoll(int32 accuracy, int32 defense);

public:

	UPROPERTY(BlueprintReadOnly)
		TArray<class AGTCharacter*> PartyCharacters;

	UPROPERTY(BlueprintReadOnly)
		TArray<class AGTCharacter*> EnemyCharacters;

	class UActionDirect* PreppedAction;
	FVector PreppedTarget;
	TArray<FVector> AreaOfEffect;

	UCombatManager();

	UFUNCTION(BlueprintPure, Category = "Combat")
		static UCombatManager* CombatManager() { return Instance; }

	//virtual class UWorld* GetWorld() const override;

	UFUNCTION(BlueprintCallable, Category = "Combat")
		static void StartCombat();

	static void StartPreCombat();

	UFUNCTION(BlueprintCallable, Category = "Combat")
		static void AdvanceInitiative();

	UFUNCTION(BlueprintPure, Category = "Combat")
		static class AGTCharacter* ActingCharacter();

	UFUNCTION(BlueprintPure, Category = "Combat")
		static class UActionAttack* GetDefaultAttack() { return Instance->DefaultAttack; }

	//DO I STILL NEED THIS?
	UFUNCTION(BlueprintCallable, Category = "Combat")
		static void InitiateActionTarget(class UAction* action, class AGTCharacter* attacker, TScriptInterface<ITargetableInterface> target);

		//DO I STILL NEED THIS?
	UFUNCTION(BlueprintCallable, Category = "Combat")
		static void InitiateActionLocation(class UAction* action, class AGTCharacter* attacker, FVector location);

	static void InitiatePreparedAction(class AGTCharacter* attacker);

	static void CompleteAction();

	//UFUNCTION(BlueprintCallable, Category = "Combat")
	//	static void InterruptAction();

	UFUNCTION(BlueprintPure, Category = "Combat")
		static bool IsPerformingActions() { return Instance->bPerformingActions; }

	UFUNCTION(BlueprintPure, Category = "Combat")
		static bool IsPreCombat() { return Instance->bPreCombat; }

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Combat")
		void SpawnTextIndicator(FVector location, const FString& text, FColor color);

	UFUNCTION(BlueprintCallable, Category = "Combat")
		static void PrepareDirectAction(class UActionDirect* action);

	static void UpdateAreaOfEffect(FVector source, FVector target);

	static bool RollAttack(class UActionAttack* action, class AGTCharacter* attacker, TScriptInterface<ITargetableInterface> target);

	static void ResetDetection(class AGTCharacter* mover);

	static void CheckDetection(class AGTCharacter* mover);
};
