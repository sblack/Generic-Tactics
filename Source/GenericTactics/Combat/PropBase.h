// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "../Utility/DirectionalSpriteInterface.h"
#include "../Utility/TargetableInterface.h"
#include "PropBase.generated.h"

/**
 * Base class for objects that can be targeted/affected in combat, but do not themselves act or attack
 */
UCLASS(ABSTRACT)
class GENERICTACTICS_API APropBase : public AActor, public ITargetableInterface, public IDirectionalSpriteInterface
{
	GENERATED_BODY()
public:
	/** Default UObject constructor. */
	APropBase(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

private:

	/** The CapsuleComponent being used for movement collision (by CharacterMovement). Always treated as being vertically aligned in simple collision check functions. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		class UCapsuleComponent* CapsuleComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		class UPaperSpriteComponent* SpriteComponent;

	FLinearColor VaryColor(FLinearColor color);

protected:
	UPROPERTY(BlueprintReadOnly)
		class UMaterialInstanceDynamic* DMI;

	/** default is base color; instance color is varied from default */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
		FLinearColor Color0;

	/** default is base color; instance color is varied from default */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
		FLinearColor Color1;

	/** default is base color; instance color is varied from default */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
		FLinearColor Color2;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
		float HueVariance;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
		float SatVariance;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
		float LumVariance;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


	void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
	/** switch to this sprite, when actor is destroyed by damage */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	class UPaperSprite* DeadSprite;

	// Called every frame
	//virtual void Tick(float DeltaTime) override;

	virtual void UpdateFacing() override;

};
