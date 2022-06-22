// Fill out your copyright notice in the Description page of Project Settings.


#include "CameraPawn.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"

ACameraPawn* ACameraPawn::Instance;

// Sets default values
ACameraPawn::ACameraPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	RootComponent->SetAbsolute(false, true, false);

	// Create a camera boom...
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->SetAbsolute(false, true, false);
	CameraBoom->TargetArmLength = 800.f;
	CameraBoom->bDoCollisionTest = false; // Don't want to pull camera in when it collides with level

	// Create a camera...
	TopDownCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("TopDownCamera"));
	TopDownCameraComponent->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	TopDownCameraComponent->bUsePawnControlRotation = false; // Camera does not rotate relative to arm
	TopDownCameraComponent->SetProjectionMode(ECameraProjectionMode::Orthographic);
}

// Called when the game starts or when spawned
void ACameraPawn::BeginPlay()
{
	Super::BeginPlay();

	Instance = this;
	CameraForward = TopDownCameraComponent->GetForwardVector() * FVector(1, 1, 0);
	CameraForward.Normalize();
	CameraRight = FVector(-CameraForward.Y, CameraForward.X, 0);
}

// Called every frame
void ACameraPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ACameraPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void ACameraPawn::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (Instance == this)
		Instance = nullptr;

	Super::EndPlay(EndPlayReason);
}

void ACameraPawn::MoveRight(float value)
{
	if (value == 0 || bAttached)
		return;

	FVector vec = CameraRight * value * PanSpeed;
	SetActorLocation(GetActorLocation() + vec, true);
}

void ACameraPawn::MoveForward(float value)
{
	if (value == 0 || bAttached)
		return;

	FVector vec = CameraForward * value * PanSpeed;
	SetActorLocation(GetActorLocation() + vec, true);
}

void ACameraPawn::ZoomIn(float value)
{
	if (value == 0)
		return;

	ZoomLength -= 64.0f * value;
	if (ZoomLength < ZoomInnerLimit)
		ZoomLength = ZoomInnerLimit;
	else if (ZoomLength > ZoomOuterLimit)
		ZoomLength = ZoomOuterLimit;

	TopDownCameraComponent->OrthoWidth = ZoomLength;
}

void ACameraPawn::RotateCamera(float value)
{
	if (value == 0)
		return;

	CameraForward = CameraForward.RotateAngleAxis(value, FVector::UpVector);
	CameraRight = CameraRight.RotateAngleAxis(value, FVector::UpVector);
	RootComponent->SetWorldRotation(CameraForward.Rotation());
}

void ACameraPawn::AttachCamera(AActor* actor)
{
	if (actor)
	{
		AttachToActor(actor, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
		RootComponent->SetWorldRotation(CameraForward.Rotation());
		bAttached = true;
	}
	else
	{
		DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		bAttached = false;
	}
}

