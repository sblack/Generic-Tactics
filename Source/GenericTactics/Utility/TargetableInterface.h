// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "GameFramework/Actor.h"
#include "GTBFL.h"
#include "TargetableInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI, BlueprintType, meta = (CannotImplementInterfaceInBlueprint)) //required for BlueprintCallable functions
class UTargetableInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class GENERICTACTICS_API ITargetableInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	/** Casts actor as a Targetable, or returns null */
	static TScriptInterface<ITargetableInterface> Targetable(AActor* actor);

	UFUNCTION(BlueprintCallable, Category = "Target")
		virtual AActor* AsActor() const { return Cast<AActor>(_getUObject()); }

	UFUNCTION(BlueprintCallable, Category = "Target")
		virtual FVector GetLocation() const { return AsActor()->GetActorLocation(); }

	UFUNCTION(BlueprintCallable, Category = "Target")
		virtual FText GetTargetName() const { return FText(); }

	UFUNCTION(BlueprintCallable, Category = "Target")
		virtual int32 GetCurrentHealth() const { return 0; }

	UFUNCTION(BlueprintCallable, Category = "Target")
		virtual int32 GetMaxHealth() const { return 1; }

	virtual int32 ApplyDamage(int32 amount, EDamageType damageType, EVitals vital) { return amount; }

	virtual void OnHit(bool bMajor);

	virtual float GetDefense() { return 10; }

	virtual void OnHoverStart() {}

	virtual void OnHoverEnd() {}
};

typedef TScriptInterface<ITargetableInterface> ITargetable;