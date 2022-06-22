// Fill out your copyright notice in the Description page of Project Settings.


#include "GTCharacter.h"
#include "GTAIController.h"
#include "../Utility/GTBFL.h"
#include "Camera/PlayerCameraManager.h"
#include "Kismet/GameplayStatics.h"
#include "PaperFlipbookComponent.h"
#include "PaperSpriteComponent.h"
#include "Materials/MaterialInstanceDynamic.h"


AGTCharacter::AGTCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//Stats = CreateDefaultSubobject<UCharacterStatsComponent>(TEXT("Stats"));
	//Equipment = CreateDefaultSubobject<UCharacterEquipmentComponent>(TEXT("Equipment"));
}

void AGTCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	if (IsPendingKill()) return;

	HairSprite = Cast<UPaperSpriteComponent>(GetDefaultSubobjectByName(TEXT("Hair")));
	HatFrontSprite = Cast<UPaperSpriteComponent>(GetDefaultSubobjectByName(TEXT("HatF")));
	HatBackSprite = Cast<UPaperSpriteComponent>(GetDefaultSubobjectByName(TEXT("HatB")));
}

void AGTCharacter::InitMaterials()
{
	BodyDMI = GetSprite()->CreateDynamicMaterialInstance(0);
	BodyDMI->SetTextureParameterValue(TEXT("Texture"), CharacterData->BodyAsset->Image);
	BodyDMI->SetVectorParameterValue(TEXT("ColorSkin"), UGTBFL::HSLtoRGB(CharacterData->SkinColorHSL));
	UColorQuartetBFL::SetColorQuartetParameter(BodyDMI, UColorQuartetBFL::HSLtoRGBQuartet(CharacterData->BodyColorsHSL));

	HairDMI = HairSprite->CreateDynamicMaterialInstance(0);
	if (CharacterData->HairIndex == 255) //bald
		HairSprite->SetVisibility(false);
	else
	{
		//HairDMI->SetTextureParameterValue(TEXT("Texture"), );
		HairDMI->SetScalarParameterValue(TEXT("Column"), CharacterData->HairIndex % 5);
		HairDMI->SetScalarParameterValue(TEXT("Row"), CharacterData->HairIndex / 5);
		HairDMI->SetVectorParameterValue(TEXT("Color"), UGTBFL::HSLtoRGB(CharacterData->HairColorHSL));
	}

	HatFrontDMI = HatFrontSprite->CreateDynamicMaterialInstance(0);
	HatBackDMI = HatBackSprite->CreateDynamicMaterialInstance(0);
	if (CharacterData->HatIndex == 255) //no hat
	{
		HatFrontSprite->SetVisibility(false);
		HatBackSprite->SetVisibility(false);
	}
	else
	{
		HatFrontDMI->SetTextureParameterValue(TEXT("Texture"), CharacterData->HatAsset->Image);
		HatBackDMI->SetTextureParameterValue(TEXT("Texture"), CharacterData->HatAsset->ImageB);
		HatFrontDMI->SetScalarParameterValue(TEXT("Column"), CharacterData->HatIndex % 5);
		HatFrontDMI->SetScalarParameterValue(TEXT("Row"), CharacterData->HatIndex / 5);
		HatBackDMI->SetScalarParameterValue(TEXT("Column"), CharacterData->HatIndex % 5);
		HatBackDMI->SetScalarParameterValue(TEXT("Row"), CharacterData->HatIndex / 5);
		FColorQuartet hatColorsRGB = UColorQuartetBFL::HSLtoRGBQuartet(CharacterData->HatColorsHSL);
		UColorQuartetBFL::SetColorQuartetParameter(HatFrontDMI, hatColorsRGB);
		UColorQuartetBFL::SetColorQuartetParameter(HatBackDMI, hatColorsRGB);
	}

	OriginalScale = GetSprite()->GetRelativeScale3D();
}

void AGTCharacter::FrontBackFlip()
{
	float diff = FMath::Fmod((GetActorRotation().Yaw - UGameplayStatics::GetPlayerCameraManager(this, 0)->GetCameraRotation().Yaw + 360.0f), 360.0f);
	bFrontView = (diff >= 90) && (diff < 270);
	bMirrored = diff >= 180;
	if(bMirrored)
		GetSprite()->SetRelativeScale3D(OriginalScale * FVector(-1, 1, 1));
	else
		GetSprite()->SetRelativeScale3D(OriginalScale);

	float frontBackDMIScalar = bFrontView ? 0 : 1;
	FName headSocket = bFrontView ? FName(TEXT("Head_F")) : FName(TEXT("Head_B"));
	BodyDMI->SetScalarParameterValue(TEXT("FrontBack"), frontBackDMIScalar);
	HairDMI->SetScalarParameterValue(TEXT("FrontBack"), frontBackDMIScalar);
	HatFrontDMI->SetScalarParameterValue(TEXT("FrontBack"), frontBackDMIScalar);
	HatBackDMI->SetScalarParameterValue(TEXT("FrontBack"), frontBackDMIScalar);
	FAttachmentTransformRules atr = FAttachmentTransformRules(EAttachmentRule::KeepRelative, true);
	HairSprite->AttachToComponent(GetSprite(), atr, headSocket);
	HatFrontSprite->AttachToComponent(GetSprite(), atr, headSocket);
	HatBackSprite->AttachToComponent(GetSprite(), atr, headSocket);
}

void AGTCharacter::BeginPlay()
{
	UE_LOG(LogTemp, Log, TEXT("BeginPlay Character"));

	if (CharacterData)
	{
		UE_LOG(LogTemp, Log, TEXT("Building from Data"));
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("Building %s from Scratch"), *GetName());
		CharacterData = UCharacterDataAsset::NewCharacter();

		if (Team != 0)
		{
			CharacterData->BodyColorsHSL = CharacterData->BodyColorsHSL.ChangeTeamHue(0, CharacterData->BodyAsset->TeamColors);
			CharacterData->HatColorsHSL = CharacterData->HatColorsHSL.ChangeTeamHue(0, CharacterData->HatAsset->TeamColors[CharacterData->HatIndex % 10]);
		}
	}

	CharacterName = CharacterData->Name;
	InitMaterials();
	FrontBackFlip();

	Super::BeginPlay();
}

void AGTCharacter::OnBeginTurn_Implementation()
{
	if (bDead)
	{
		OnEndTurn();
		return;
	}
	CurrentAP += 5;
	/*CurrentAP += Stats->MaxAP.TotalValue;
	if (CurrentAP > Stats->MaxAP.TotalValue * 1.5f)
		CurrentAP = Stats->MaxAP.TotalValue * 1.5f;*/
}

void AGTCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

FORCEINLINE class AGTAIController* AGTCharacter::GetTacticsAI()
{
	return Cast<AGTAIController>(GetController());
}

int32 AGTCharacter::GetCurrentHealth() const
{
	/*int32* health = Stats->CurrentVitals.Find(EVitals::Health);
	if (health != nullptr)
		return *health;*/

	return 0;
}

int32 AGTCharacter::GetMaxHealth() const
{
	/*FStatsInt* health = Stats->Vitals.Find(EVitals::Health);
	if (health != nullptr)
		return health->TotalValue;*/

	return 0;
}

float AGTCharacter::RollInitiative()
{
	return FMath::RandRange(0, 20);
}

int32 AGTCharacter::ApplyDamage(int32 amount, EDamageType damageType, EVitals vital)
{
	UE_LOG(LogTemp, Log, TEXT("Apply Damage: %d"), amount);
	UCombatManager::CombatManager()->SpawnTextIndicator(GetActorLocation(), FString::FromInt(amount), FColor::Red);

	/*Stats->CurrentVitals[vital] -= amount;
	if (Stats->CurrentVitals[vital] <= 0)
	{
		Stats->CurrentVitals[vital] = 0;
		switch (vital)
		{
		case EVitals::Health:
			bDead = true;
			CurrentAP = 0;
			AnimState = EAnimState::Down;
			break;
		case EVitals::Stamina:
			break;
		case EVitals::Mana:
			break;
		default:
			break;
		}
	}*/


	return amount;
}

void AGTCharacter::OnHit(bool bMajor)
{
	ITargetableInterface::OnHit(bMajor);
	//PlayHitAnim(bMajor);
}

void AGTCharacter::OnHoverStart()
{
	GetSprite()->SetSpriteColor(FLinearColor(.5f, .5f, 0));
}

void AGTCharacter::OnHoverEnd()
{
	GetSprite()->SetSpriteColor(FLinearColor(0, 0, 0));
}

void AGTCharacter::StartMoving(TArray<FVector> path)
{
	bIsMoving = true;
	MoveTimePassed = 0;
	MoveSteps.Empty(path.Num() - 1);
	for (int i = 0; i < path.Num() - 1; i++)
	{
		MoveSteps.Add(FMovementStep(path[i], path[i + 1]));
	}
	SetActorRotation((MoveSteps[0].Velocity * FVector(1, 1, 0)).ToOrientationRotator());
}

void AGTCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bIsMoving)
	{
		if (MoveSteps.Num() == 0)
		{
			bIsMoving = false;
			GetTacticsAI()->FinishedMoving();
			return;
			//DONE
		}
		else
		{
			MoveTimePassed += DeltaTime * 400;
			if (MoveTimePassed >= MoveSteps[0].TimeToArrival)
			{
				if (MoveSteps.Num() == 1)
				{
					bIsMoving = false;
					SetActorLocation(MoveSteps[0].CalcPosition(MoveSteps[0].TimeToArrival));
					GetTacticsAI()->FinishedMoving();
					return;
					//DONE
				}
				MoveTimePassed -= MoveSteps[0].TimeToArrival;
				MoveSteps.RemoveAt(0);
				SetActorRotation((MoveSteps[0].Velocity * FVector(1, 1, 0)).ToOrientationRotator());
			}

			SetActorLocation(MoveSteps[0].CalcPosition(MoveTimePassed));
		}
	}
}

//#if WITH_EDITOR
//void AGTCharacter::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
//{
//	Super::PostEditChangeProperty(PropertyChangedEvent);
//}
//#endif

#define GRAV .0098f

FMovementStep::FMovementStep(FVector start, FVector end)
{
	StartLocation = start + FVector(0, 0, 80); //adjust for Z position of character
	TimeToArrival = FVector::DistXY(end, start);
	if (FMath::Abs(start.Z - end.Z) < 25)
	{
		bJump = false;
		Velocity = (end - start) / TimeToArrival;
	}
	else
	{
		bJump = true;
		Velocity = (end - start) * FVector(1, 1, 0) / TimeToArrival
			+ FVector(0, 0, (end.Z - start.Z + GRAV / 2 * TimeToArrival * TimeToArrival) / TimeToArrival);
	}
}

FVector FMovementStep::CalcPosition(float time)
{
	if (bJump)
	{
		return StartLocation + Velocity * time + FVector(0, 0, -GRAV / 2 * time * time);
	}
	else
	{
		return StartLocation + Velocity * time;
	}
}
