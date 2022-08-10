// Fill out your copyright notice in the Description page of Project Settings.


#include "GTCharacter.h"
#include "GTAIController.h"
#include "../Combat/Action.h"
//#include "../Combat/CombatManager.h"
//#include "../Movement/NavGrid.h"
#include "../Player/CameraPawn.h"
#include "../Player/GTPlayerController.h"
#include "../UI/GTHUDCode.h"
#include "Camera/PlayerCameraManager.h"
#include "Kismet/GameplayStatics.h"
#include "PaperFlipbookComponent.h"
#include "PaperSpriteComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "TimerManager.h"


bool AGTCharacter::UseFullGuard()
{
	return (CharacterData->Shield != nullptr);
}

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
	ShieldSprite = Cast<UPaperSpriteComponent>(GetDefaultSubobjectByName(TEXT("Shield")));
	WeaponSprite = Cast<UPaperSpriteComponent>(GetDefaultSubobjectByName(TEXT("Weapon")));
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
	float bodyFB, headFB, headFlip, weaponFlip;
	FName headSocket, shieldSocket, weaponSocket;
	/*if (diff < 90) {
		bodyFB = 3; headFB = 1; headFlip = -1; weaponFlip = 1;
		headSocket = FName(TEXT("Head_3")); shieldSocket = FName(TEXT("Shield_3")); weaponSocket = FName(TEXT("Weapon_3"));
	}
	else if (diff < 180) {
		bodyFB = 0; headFB = 0; headFlip = 1; weaponFlip = 1;
		headSocket = FName(TEXT("Head_0")); shieldSocket = FName(TEXT("Shield_0")); weaponSocket = FName(TEXT("Weapon_0"));
	}
	else if (diff < 270) {
		bodyFB = 1; headFB = 0; headFlip = -1; weaponFlip = -1;
		headSocket = FName(TEXT("Head_1")); shieldSocket = FName(TEXT("Shield_1")); weaponSocket = FName(TEXT("Weapon_1"));
	}
	else {
		bodyFB = 2; headFB = 1; headFlip = 1; weaponFlip = -1;
		headSocket = FName(TEXT("Head_2")); shieldSocket = FName(TEXT("Shield_2")); weaponSocket = FName(TEXT("Weapon_2"));
	}*/
	if (diff < 90) {
		bodyFB = 2; headFB = 1; headFlip = 1; weaponFlip = -1;
		headSocket = FName(TEXT("Head_2")); shieldSocket = FName(TEXT("Shield_2")); weaponSocket = FName(TEXT("Weapon_2"));
	}
	else if (diff < 180) {
		bodyFB = 1; headFB = 0; headFlip = -1; weaponFlip = -1;
		headSocket = FName(TEXT("Head_1")); shieldSocket = FName(TEXT("Shield_1")); weaponSocket = FName(TEXT("Weapon_1"));
	}
	else if (diff < 270) {
		bodyFB = 0; headFB = 0; headFlip = 1; weaponFlip = 1;
		headSocket = FName(TEXT("Head_0")); shieldSocket = FName(TEXT("Shield_0")); weaponSocket = FName(TEXT("Weapon_0"));
	}
	else {
		bodyFB = 3; headFB = 1; headFlip = -1; weaponFlip = 1;
		headSocket = FName(TEXT("Head_3")); shieldSocket = FName(TEXT("Shield_3")); weaponSocket = FName(TEXT("Weapon_3"));
	}

	BodyDMI->SetScalarParameterValue(TEXT("FrontBack"), bodyFB);
	HairDMI->SetScalarParameterValue(TEXT("FrontBack"), headFB);
	HatFrontDMI->SetScalarParameterValue(TEXT("FrontBack"), headFB);
	HatBackDMI->SetScalarParameterValue(TEXT("FrontBack"), headFB);
	FAttachmentTransformRules atr = FAttachmentTransformRules(EAttachmentRule::KeepRelative, true);
	HairSprite->AttachToComponent(GetSprite(), atr, headSocket);
	HatFrontSprite->AttachToComponent(GetSprite(), atr, headSocket);
	HatBackSprite->AttachToComponent(GetSprite(), atr, headSocket);

	WeaponSprite->AttachToComponent(GetSprite(), atr, weaponSocket);
	ShieldSprite->AttachToComponent(GetSprite(), atr, shieldSocket);

	HairSprite->SetRelativeScale3D(FVector(headFlip, 1, 1));
	HatFrontSprite->SetRelativeScale3D(FVector(headFlip, 1, 1));
	HatBackSprite->SetRelativeScale3D(FVector(headFlip, 1, 1));

	WeaponSprite->SetRelativeScale3D(FVector(weaponFlip, 1, 1));
	ShieldSprite->SetRelativeScale3D(FVector(weaponFlip, 1, 1));
}

void AGTCharacter::BeginPlay()
{
	UE_LOG(LogTemp, Log, TEXT("BeginPlay Character"));

	if (CharacterData)
	{
		UE_LOG(LogTemp, Log, TEXT("Building from Data"));

		CharacterData = UCharacterDataAsset::RandomCopyCharacter(CharacterData, 0);
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("Building %s from Scratch"), *GetName());
		CharacterData = UCharacterDataAsset::NewCharacter();

		if (!IsPartyCharacter())
		{
			CharacterData->BodyColorsHSL = CharacterData->BodyColorsHSL.ChangeTeamHue(0, CharacterData->BodyAsset->TeamColors);
			CharacterData->HatColorsHSL = CharacterData->HatColorsHSL.ChangeTeamHue(0, CharacterData->HatAsset->TeamColors[CharacterData->HatIndex % 10]);
		}
	}

	CharacterName = CharacterData->Name;
	InitMaterials();
	FrontBackFlip();

	Initiative = RollInitiative();
	MoveDataID = -1;
	bIsMyTurn = false;

	Super::BeginPlay();
}

void AGTCharacter::BeginTurn_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("Begin Turn %s"), *GetName());
	if (bDead)
	{
		UE_LOG(LogTemp, Log, TEXT("%s is dead, ending turn"), *GetName());
		EndTurn();
		return;
	}
	CurrentAP = 5;
	/*CurrentAP += Stats->MaxAP.TotalValue;
	if (CurrentAP > Stats->MaxAP.TotalValue * 1.5f)
		CurrentAP = Stats->MaxAP.TotalValue * 1.5f;*/

	bIsMyTurn = true;
	if (ANavGrid::Instance == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("No Tactics Grid"));
		return;
	}
	if (ACameraPawn::Instance == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("No Camera"));
		return;
	}
	if (UGTHUDCode::Instance == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("No HUD"));
		return;
	}

	ANavGrid::MoveDataID++;
	ACameraPawn::Instance->SetActorLocation(GetActorLocation());
	UGTHUDCode::Instance->ShowCharacterInfo(this);

	if(IsPartyCharacter())
	{
		AGTPlayerController::Instance->ControlCharacter(this);
		ANavGrid::Instance->ShowMoveRange(this);
		UGTHUDCode::Instance->ShowHideCommandMenu(this);
	}
	else
		ANavGrid::Instance->GenerateMoveData(this);


	if (CharacterData && !IsPartyCharacter()) //TODO: allow for AI control on Party Characters?
	{
		bool bEndTurn = true; //if AI can't perform any objectives, end turn
		for (int i = 0; i < CharacterData->AIObjectives.Num(); i++)
		{
			if (CharacterData->AIObjectives[i]->Attempt(this))
			{
				bEndTurn = false;
				UE_LOG(LogTemp, Log, TEXT("%s successful"), *CharacterData->AIObjectives[i]->GetDebugString());
				break;
			}
			else
			{
				UE_LOG(LogTemp, Log, TEXT("%s failed"), *CharacterData->AIObjectives[i]->GetDebugString());
			}
		}

		if (bEndTurn)
		{
			UE_LOG(LogTemp, Log, TEXT("all objectives impossible, ending turn"));

			FTimerHandle TimerHandle;
			GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &AGTCharacter::EndTurn, 1, false);
		}
	}
}

void AGTCharacter::EndTurn_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("End Turn %s"), *GetName());
	bIsMyTurn = false;

	if(IsPartyCharacter())
	{
		ANavGrid::Instance->ShowMoveRange(nullptr);
		UGTHUDCode::Instance->ShowHideCommandMenu(nullptr);
	}

	UCombatManager::AdvanceInitiative();
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

void AGTCharacter::ResolveAction()
{
	if (ActionInProgress.Action)
		ActionInProgress.Action->Resolve(this, ActionInProgress.Location);
	else
		CompleteAction();
}

void AGTCharacter::CompleteAction()
{
	if (IsPartyCharacter())
	{
		ANavGrid::Instance->ShowMoveRange(this);
		UGTHUDCode::Instance->ShowHideCommandMenu(this);
	}
	else
		ANavGrid::Instance->GenerateMoveData(this);


	if (CharacterData && !IsPartyCharacter()) //TODO: allow for AI control on Party Characters?
	{
		bool bEndTurn = true; //if AI can't perform any objectives, end turn
		for (int i = 0; i < CharacterData->AIObjectives.Num(); i++)
		{
			if (CharacterData->AIObjectives[i]->Attempt(this))
			{
				bEndTurn = false;
				UE_LOG(LogTemp, Log, TEXT("%s successful"), *CharacterData->AIObjectives[i]->GetDebugString());
				break;
			}
			else
			{
				UE_LOG(LogTemp, Log, TEXT("%s failed"), *CharacterData->AIObjectives[i]->GetDebugString());
			}
		}

		if (bEndTurn)
		{
			UE_LOG(LogTemp, Log, TEXT("all objectives impossible, ending turn"));

			FTimerHandle TimerHandle;
			GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &AGTCharacter::EndTurn, 1, false);
		}
	}
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

bool AGTCharacter::GetPathTo(FVector destination, FNavPath& path)
{
	UE_LOG(LogTemp, Log, TEXT("%s GetPathTo %s"), *GetName(), *destination.ToString());
	destination = ANavGrid::Instance->AlignToGrid(destination);
	const FNodeData* data = FindMoveData(destination);
	if (data == nullptr)
	{
		//destination may not be in movedata due to range, so no error message
		UE_LOG(LogTemp, Log, TEXT("PathBack failed: destination %s not found in MoveData"), *destination.ToCompactString());
		UE_LOG(LogTemp, Log, TEXT("MoveGrid.Num() = %d"), MoveGrid.Num());
		return false;
	}

	path.Path.Emplace(destination);
	path.Cost = data->TotalCost;

	int sanity = 0;
	while (data->TotalCost != 0)
	{
		//UE_LOG(LogTemp, Log, TEXT("PathBack: %s %f"), *data->Location.ToCompactString(), data->Value);
		data = FindMoveData(data->Origin);
		if (data == nullptr)
		{
			UE_LOG(LogTemp, Error, TEXT("PathBack failed: origin not found in MoveData"));
			return false;
		}

		path.Path.EmplaceAt(0, data->Location);

		if (++sanity > 100)
		{
			UE_LOG(LogTemp, Error, TEXT("PathBack failed: sanity"));
			return false;
		}
	}

	if(IsPartyCharacter())
		ANavGrid::Instance->ShowPath(path.Path);

	return true;
}

void AGTCharacter::StartMoving(FNavPath path)
{
	CurrentAP -= path.Cost;
	bIsMoving = true;
	MoveTimePassed = 0;
	MoveSteps.Empty(path.Path.Num() - 1);
	for (int i = 0; i < path.Path.Num() - 1; i++)
	{
		MoveSteps.Add(FMovementStep(path.Path[i], path.Path[i + 1]));
	}
	SetActorRotation((MoveSteps[0].Velocity * FVector(1, 1, 0)).ToOrientationRotator());
	ANavGrid::RemoveActorFromGrid(this);
	ACameraPawn::Instance->AttachCamera(this);
}

TArray<FNodeData> AGTCharacter::GetReachableArea()
{
	TArray<FNodeData> result;
	if (!ANavGrid::Instance)
	{
		UE_LOG(LogTemp, Error, TEXT("ANavGrid Instance is null"));
		return result;
	}
	ANavGrid::Instance->GenerateMoveData(this);

	for (int i = 0; i < MoveGrid.Num(); i++)
	{
		for (int j = 0; j < MoveGrid[i].Num(); j++)
		{

			if (!MoveGrid[i][j].Occupied && MoveGrid[i][j].TotalCost <= CurrentAP)
				result.Add(MoveGrid[i][j]);
		}
	}

	return result;
}

FNodeData AGTCharacter::NearestReachableLocationToTarget(FVector target, float range)
{
	float minCost = 999;
	float minDist = 999;
	FNodeData location;
	TArray<FNodeData> reachable = GetReachableArea();
	for (int i = 0; i < reachable.Num(); i++)
	{
		//UE_LOG(LogTemp, Log, TEXT("C: %f D: %f %s"), minCost, minDist, *reachable[i].Location.ToString());
		float dist = FMath::Abs(ANavGrid::Instance->GetDistance(target - reachable[i].Location) - range);
		if (dist < minDist)
		{
			minDist = dist;
			minCost = reachable[i].TotalCost;
			location = reachable[i];
		}
		else if (dist == minDist && reachable[i].TotalCost < minCost)
		{
			minCost = reachable[i].TotalCost;
			location = reachable[i];
		}
	}

	if (minCost == 999)
	{
		UE_LOG(LogTemp, Error, TEXT("Could not find a location"));
		return FNodeData(location);
	}
	//UE_LOG(LogTemp, Log, TEXT("NRLtT: %s"), *location.Location.ToString());
	return location;
}

bool AGTCharacter::IsSameTeam(ITargetable target)
{
	AGTCharacter* targetCharacter = Cast<AGTCharacter>(target->AsActor());
	if (!targetCharacter)
		return false;

	return targetCharacter->Team == Team;
}

void AGTCharacter::QueueAction(FNavPath path, FActionData actionData)
{
	if (path.Path.Num() == 0)
	{
		UE_LOG(LogTemp, Error, TEXT("Path is empty"));
		EndTurn();
		return;
	}
	else if (path.Path.Num() == 1) //assuming that location should be the character's current location, thus don't need to move
	{
		ActionInProgress = actionData;
		if (ActionInProgress.Action)
		{
			ActionInProgress.Action->Perform(this, ActionInProgress.Location);
		}
		else
			CompleteAction();

		return;
	}

	StartMoving(path);
	ActionInProgress = actionData;
}

TArray<class UActionAttack*> AGTCharacter::GetAllAttacks()
{
	TArray<UActionAttack*> attacks;
	if (DefaultMeleeAttack)
		attacks.Add(DefaultMeleeAttack);
	if (DefaultRangedAttack)
		attacks.Add(DefaultRangedAttack);
	//TODO: get weapon attacks
	//TODO: get skill attacks


	return attacks;
}

void AGTCharacter::FinishedMoving()
{
	if (IsPendingKill())
	{
		return;
	}

	if (GetGameInstance()->IsPendingKillOrUnreachable())
	{
		return;
	}

	SetActorLocation(ANavGrid::Instance->AlignToGrid(GetActorLocation()));
	ANavGrid::AddActorToGrid(this);
	ANavGrid::MoveDataID++;
	ACameraPawn::Instance->AttachCamera(nullptr);
	if(IsPartyCharacter())
		AGTPlayerController::Instance->MoveCompleted();

	if (ActionInProgress.Action)
	{
		ActionInProgress.Action->Perform(this, ActionInProgress.Location);
	}
	else
		CompleteAction();
}

const FNodeData* AGTCharacter::FindMoveData(FVector vec) const
{
	for (int i = 0; i < MoveGrid.Num(); i++)
	{
		if (MoveGrid[i].Num() == 0)
		{
			continue;
		}

		if (MoveGrid[i][0].Location.X == vec.X)
		{
			for (int j = 0; j < MoveGrid[i].Num(); j++)
			{
				if (MoveGrid[i][j].Location.Y == vec.Y)
					return &(MoveGrid[i][j]);
			}

			return nullptr;
		}
	}

	return nullptr;
}

void AGTCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bIsMoving)
	{
		if (MoveSteps.Num() == 0)
		{
			bIsMoving = false;
			FinishedMoving();
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
					FinishedMoving();
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
