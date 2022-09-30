// Fill out your copyright notice in the Description page of Project Settings.


#include "PropBase.h"
#include "../Utility/GTBFL.h"
#include "PaperSpriteComponent.h"
#include "Engine/CollisionProfile.h"
#include "Components/CapsuleComponent.h"

APropBase::APropBase(const FObjectInitializer& ObjectInitializer /*= FObjectInitializer::Get()*/)
{
	//PrimaryActorTick.bCanEverTick = true;

	CapsuleComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CollisionCylinder"));
	CapsuleComponent->InitCapsuleSize(50.0f, 100.0f);
	CapsuleComponent->SetCollisionProfileName(UCollisionProfile::BlockAllDynamic_ProfileName);

	CapsuleComponent->CanCharacterStepUpOn = ECB_No;
	CapsuleComponent->SetShouldUpdatePhysicsVolume(true);
	CapsuleComponent->SetCanEverAffectNavigation(false);
	CapsuleComponent->bDynamicObstacle = true;
	RootComponent = CapsuleComponent;

	SpriteComponent = CreateDefaultSubobject<UPaperSpriteComponent>(TEXT("Sprite"));
	SpriteComponent->SetupAttachment(CapsuleComponent);
}

FLinearColor APropBase::VaryColor(FLinearColor color)
{
	float h, s, l, a;
	UGTBFL::RGBToHSL(color, h, s, l, a);
	h += FMath::RandRange(-HueVariance, HueVariance);
	s += FMath::RandRange(-SatVariance, SatVariance);
	l += FMath::RandRange(-LumVariance, LumVariance);

	return UGTBFL::HSLtoRGB(h, s, l, a);
}

void APropBase::BeginPlay()
{
	DMI = SpriteComponent->CreateDynamicMaterialInstance(0);
	Color0 = VaryColor(Color0);
	Color1 = VaryColor(Color1);
	Color2 = VaryColor(Color2);
	DMI->SetVectorParameterValue(TEXT("Color0"), Color0);
	DMI->SetVectorParameterValue(TEXT("Color1"), Color1);
	DMI->SetVectorParameterValue(TEXT("Color2"), Color2);

	IDirectionalSpriteInterface::Sprites.Add(this);
}

void APropBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	IDirectionalSpriteInterface::Sprites.Remove(this);
}

//void APropBase::Tick(float DeltaTime)
//{
//	Super::Tick(DeltaTime);
//}

void APropBase::UpdateFacing()
{
	float diff = FMath::Fmod((GetActorRotation().Yaw - UGameplayStatics::GetPlayerCameraManager(this, 0)->GetCameraRotation().Yaw + 360.0f), 360.0f);
	float angle;
	if (diff < 90) {
		angle = 2;
	}
	else if (diff < 180) {
		angle = 1;
	}
	else if (diff < 270) {
		angle = 0;
	}
	else {
		angle = 3;
	}

	DMI->SetScalarParameterValue(TEXT("Angle"), angle);
	UGTBFL::FaceCamera(this, SpriteComponent);
}
