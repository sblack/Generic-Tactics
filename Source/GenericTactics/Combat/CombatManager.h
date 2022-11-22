// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "../Utility/GTBFL.h"
#include "../Utility/ActionSourceInterface.h"
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
		TScriptInterface<IActionSourceInterface> Source;

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
	static UWorld* BackupWorld;

	UPROPERTY(BlueprintReadWrite)
		TArray<TScriptInterface<IActionSourceInterface>> InitiativeQueue;

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

	UFUNCTION(BlueprintImplementableEvent, Category = "Combat")
		ESuccessLevel AttackRoll(int32 accuracy, int32 defense);

public:
	virtual class UWorld* GetWorld() const override;

	UFUNCTION(BlueprintPure, Category = "Combat")
		static UCombatManager* CombatManager() { return Instance; }

	UPROPERTY(BlueprintReadOnly)
		TArray<class AGTCharacter*> PartyCharacters;

	UPROPERTY(BlueprintReadOnly)
		TArray<class AGTCharacter*> EnemyCharacters;

	/** param bGetParty get PartyCharacters if true, EnemyCharacters if false */
	UFUNCTION(BlueprintPure)
		static TArray<class AGTCharacter*> GetTeam(bool bGetParty);

	class UActionDirect* PreppedAction;
	FVector PreppedTarget;
	TArray<FVector> AreaOfEffect;

	UCombatManager();

	UFUNCTION(BlueprintCallable, Category = "Combat")
		static void StartCombat();

	static void StartPreCombat();

	UFUNCTION(BlueprintCallable, Category = "Combat")
		static void AdvanceInitiative();

	UFUNCTION(BlueprintPure, Category = "Combat")
		static class UActionAttack* GetDefaultAttack() { return Instance->DefaultAttack; }

	//DO I STILL NEED THIS?
	UFUNCTION(BlueprintCallable, Category = "Combat", meta = (DeprecatedFunction))
		static void InitiateActionTarget(class UAction* action, class AGTCharacter* attacker, TScriptInterface<ITargetableInterface> target);

		//DO I STILL NEED THIS?
	UFUNCTION(BlueprintCallable, Category = "Combat", meta = (DeprecatedFunction))
		static void InitiateActionLocation(class UAction* action, class AGTCharacter* attacker, FVector location);

	static void InitiatePreparedAction(class AGTCharacter* attacker);

	//UFUNCTION(BlueprintCallable, Category = "Combat")
	//	static void InterruptAction();

	UFUNCTION(BlueprintPure, Category = "Combat")
		static bool IsPerformingActions() { return Instance->bPerformingActions; }

	UFUNCTION(BlueprintPure, Category = "Combat")
		static bool IsPreCombat() { return Instance->bPreCombat; }

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Combat")
		void SpawnTextIndicator(FVector location, const FString& text, FLinearColor color, float scale = 1);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Combat")
		void SpawnDamageIndicator(FVector location, int32 damage, ESuccessLevel success);

	UFUNCTION(BlueprintCallable, Category = "Combat")
		static void PrepareDirectAction(class UActionDirect* action);

	static void UpdateAreaOfEffect(FVector source, FVector target);

	static ESuccessLevel RollAttack(class UActionAttack* action, TScriptInterface<IActionSourceInterface> source, TScriptInterface<ITargetableInterface> target);

	static void ResetDetection(class AGTCharacter* mover);

	static void CheckDetection(class AGTCharacter* mover);

	UFUNCTION(BlueprintCallable, Category = "Combat")
		static void EndCurrentTurn();
};
