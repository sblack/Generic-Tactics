// Fill out your copyright notice in the Description page of Project Settings.


#include "Projectile.h"
#include "Action.h"
#include "../Character/GTCharacter.h"

// Sets default values
AProjectile::AProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AProjectile::BeginPlay()
{
	Super::BeginPlay();
	StartLocation = GetActorLocation();
}

//#define GRAV .0098f
#define GRAV2 3920.0f

// Called every frame
void AProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	TotalTime += DeltaTime;

	if (bBallistic)
	{
		SetActorLocation(StartLocation + Velocity * TotalTime + FVector(0, 0, -GRAV2 / 2 * TotalTime * TotalTime));
		//UE_LOG(LogTemp, Log, TEXT("Position: %s at %f"), *GetActorLocation().ToString(), TotalTime);
	}
	else
	{
		SetActorLocation(GetActorLocation() + Velocity * DeltaTime * Speed);
		Velocity += (Target - GetActorLocation()).GetSafeNormal() * DeltaTime * TotalTime;
		Velocity.Normalize();
	}

	if (FVector::DistSquared(GetActorLocation(), Target) < 50 * 50)
	{
		//Action->Resolve(Source, Target);
		Source->ResolveAction();
		Destroy();
	}
	else if (GetActorLocation().Z < -100)
	{
		UE_LOG(LogTemp, Warning, TEXT("Projectile went under Z=-100. Killing."));
		//Action->Resolve(Source, Target);
		Source->ResolveAction();
		Destroy();
	}
}

void AProjectile::Init(class UAction* action, class AGTCharacter* source, FVector target)
{
	Action = action;
	Source = source;

	Target = target + FVector(0, 0, 80); //aim toward center of character instead of foot
	//UE_LOG(LogTemp, Log, TEXT("Target: %s"), *Target.ToString());
	float eta = FMath::Sqrt(FVector::Dist2D(Target, GetActorLocation())) / 40; //move faster for farther targets
	if (bBallistic)
	{
		FVector diff = target - GetActorLocation();
		//UE_LOG(LogTemp, Log, TEXT("GRAV: %f"), GRAV2);
		//UE_LOG(LogTemp, Log, TEXT("Diff: %s"), *diff.ToString());
		Velocity = FVector(diff.X / eta, diff.Y / eta, (diff.Z + GRAV2 * eta * eta / 2) / eta);
		//UE_LOG(LogTemp, Log, TEXT("Velocity: %s"), *Velocity.ToString());
	}
	else
	{
		Velocity = FMath::VRandCone(FVector(0, 0, 1), PI / 4, PI / 4);
		Speed = eta;
	}
}
