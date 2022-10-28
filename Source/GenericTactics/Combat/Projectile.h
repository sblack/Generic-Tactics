// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Projectile.generated.h"

UCLASS(ABSTRACT)
class GENERICTACTICS_API AProjectile : public AActor
{
	GENERATED_BODY()
protected:
	/** true - parabolic motion; false - magic homing */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	bool bBallistic;

	UPROPERTY(BlueprintReadOnly)
	class UAction* Action;

	UPROPERTY(BlueprintReadOnly)
	class AGTCharacter* Source;

	UPROPERTY(BlueprintReadOnly)
	FVector Target;

	UPROPERTY(BlueprintReadWrite)
	FVector Velocity;

	/** property serves as speed multiplier */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	float Speed = 1;

	FVector StartLocation;

	UPROPERTY(BlueprintReadOnly)
	float TotalTime = 0;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintNativeEvent)
		void UpdatePosition(float DeltaTime);

	void UpdatePosition_Implementation(float DeltaTime);

public:
	// Sets default values for this actor's properties
	AProjectile();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void Init(class UAction* action, class AGTCharacter* source, FVector target);
};
