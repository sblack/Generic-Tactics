// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "CameraPawn.generated.h"

UCLASS(ABSTRACT)
class GENERICTACTICS_API ACameraPawn : public APawn
{
	GENERATED_BODY()

protected:
	/** Lateral forward */
	FVector CameraForward;
	/** Lateral right */
	FVector CameraRight;

	/** Top down camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class UCameraComponent* TopDownCameraComponent;

	/** Camera boom positioning the camera above the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class USpringArmComponent* CameraBoom;

	FRotator VerticalAngle = FRotator(-60, 0, 0);
	bool bAttached;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	static ACameraPawn* Instance;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera")
		float ZoomInnerLimit = 512.0f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera")
		float ZoomOuterLimit = 2048.0f;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
		float ZoomLength = 1024.0f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera")
		float PanSpeed = 1000.0f;

	/** Center of player starting area. */
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Camera")
		FVector StartingAreaCenter;
	/** Bounds of player starting area. */
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Camera")
		FVector StartingAreaBounds;
	// Sets default values for this pawn's properties
	ACameraPawn();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;


	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UFUNCTION(BlueprintPure, Category = "Camera")
		static ACameraPawn* CameraPawn() { return Instance; }

	UFUNCTION(BlueprintCallable, Category = "Camera")
		void MoveRight(float value);

	UFUNCTION(BlueprintCallable, Category = "Camera")
		void MoveForward(float value);

	UFUNCTION(BlueprintCallable, Category = "Camera")
		void ZoomIn(float value);

	UFUNCTION(BlueprintCallable, Category = "Camera")
		void RotateCamera(float value);

	void AttachCamera(AActor* actor);

	UFUNCTION(BlueprintImplementableEvent, Category = "Camera")
		void RevealMap();
};
