// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "GameFramework/Actor.h"
#include "GTBFL.h"
#include "ActionSourceInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI, BlueprintType, meta = (CannotImplementInterfaceInBlueprint)) //required for BlueprintCallable functions
class UActionSourceInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class GENERICTACTICS_API IActionSourceInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	/** Casts actor as a Targetable, or returns null */
	static TScriptInterface<IActionSourceInterface> ActionSource(AActor* actor);

	UFUNCTION(BlueprintCallable, Category = "Action")
		virtual AActor* AsActor() const { return Cast<AActor>(_getUObject()); }

	UFUNCTION(BlueprintCallable, Category = "Action")
		virtual FVector GetLocation() const { return AsActor()->GetActorLocation(); }

	UFUNCTION(BlueprintCallable, Category = "Action")
		virtual FText GetActionSourceName() const { return FText(); }

	UFUNCTION(BlueprintCallable, Category = "Action")
		virtual int32 GetAccuracy(EAttackType attack) const { return 0; }

	virtual float GetInitiative() { return 0; }

	virtual void OnHoverStart() {}

	virtual void OnHoverEnd() {}

	virtual void BeginTurn() {}

	//make sure to call UCombatManager::AdvanceInitiative()
	virtual void EndTurn();
};

typedef TScriptInterface<IActionSourceInterface> IActionSource;
