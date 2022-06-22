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
	UPROPERTY(BlueprintReadOnly)
		class UAction* Action;

	UPROPERTY(BlueprintReadOnly)
		class AGTCharacter* Actor;

	UPROPERTY(BlueprintReadOnly)
		TScriptInterface<ITargetableInterface> Target;

	UPROPERTY(BlueprintReadOnly)
		FVector RelativeLocation;

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
		TArray<class AGTAIController*> InitiativeQueue;

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

	void AdvanceActionQueue();

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
		static class AGTAIController* ActingController();

	UFUNCTION(BlueprintPure, Category = "Combat")
		static class UActionAttack* GetDefaultAttack() { return Instance->DefaultAttack; }

	UFUNCTION(BlueprintCallable, Category = "Combat")
		static void InitiateActionTarget(class UAction* action, class AGTCharacter* attacker, TScriptInterface<ITargetableInterface> target);

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
};
