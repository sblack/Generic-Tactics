// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "DirectionalSpriteInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI, BlueprintType, meta = (CannotImplementInterfaceInBlueprint)) //required for BlueprintCallable functions
class UDirectionalSpriteInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * anything with a sprite
 */
class GENERICTACTICS_API IDirectionalSpriteInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	static TArray<TScriptInterface<IDirectionalSpriteInterface>> Sprites;

	/** Casts actor as a Targetable, or returns null */
	static TScriptInterface<IDirectionalSpriteInterface> DirectionalSprite(AActor* actor);

	UFUNCTION(BlueprintCallable, Category = "Sprites")
		virtual AActor* AsActor() const { return Cast<AActor>(_getUObject()); }

	UFUNCTION(BlueprintCallable, Category = "Sprites")
		virtual FVector GetLocation() const { return AsActor()->GetActorLocation(); }

	UFUNCTION(BlueprintCallable, Category = "Sprites")
		virtual void UpdateFacing() {}
};

typedef TScriptInterface<IDirectionalSpriteInterface> IDirectionalSprite;