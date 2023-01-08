// Fill out your copyright notice in the Description page of Project Settings.


#include "GTCharacter.h"
#include "GTAIController.h"
#include "StatsBlock.h"
#include "../Combat/Action.h"
#include "../Combat/ActionAttack.h"
#include "../Combat/CombatManager.h"
#include "../Combat/Projectile.h"
#include "../Items/ItemEquipment.h"
//#include "../Movement/NavGrid.h"
#include "../Player/CameraPawn.h"
#include "../Player/GTPlayerController.h"
#include "../UI/GTHUDCode.h"
#include "Camera/PlayerCameraManager.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"
#include "PaperFlipbookComponent.h"
#include "PaperSpriteComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Particles/ParticleSystem.h"
#include "Particles/ParticleSystemComponent.h"
#include "TimerManager.h"
#include "PaperZDAnimInstance.h"
#include "AnimSequences\PaperZDAnimSequence.h"
#include "AnimSequences\Players\PaperZDAnimPlayer.h"

//INHERITED
void AGTCharacter::BeginPlay()
{
	UE_LOG(LogTemp, Log, TEXT("BeginPlay Character"));

	InitMaterials();
	IDirectionalSpriteInterface::Sprites.Add(this);

	Stats = NewObject<UStatsBlock>(this, TEXT("Stats"));
	SetStats();

	Initiative = RollInitiative() + Stats->InitiativeBonus + Stats->Stats[ECharStat::Dex];
	MoveDataID = -1;
	bIsMyTurn = false;

	Super::BeginPlay();
}

FORCEINLINE class AGTAIController* AGTCharacter::GetTacticsAI()
{
	return Cast<AGTAIController>(GetController());
}

AGTCharacter::AGTCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//Stats = CreateDefaultSubobject<UCharacterStatsComponent>(TEXT("Stats"));
	//Equipment = CreateDefaultSubobject<UCharacterEquipmentComponent>(TEXT("Equipment"));

	GetCapsuleComponent()->SetCollisionResponseToAllChannels(ECR_Ignore);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera, ECR_Block);
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
				OnMoveStep();
			}
			else
				SetActorLocation(MoveSteps[0].CalcPosition(MoveTimePassed));
		}
	}
	else if (LungeTime >= 0)
	{
		LungeTime -= DeltaTime;
		if (LungeTime < 0)
			SetActorLocation(PreLungePosition);
		else
		{
			SetActorLocation(FMath::Lerp(PreLungePosition, LungeDestination, 4 * LungeTime * (1 - LungeTime)));
		}
		UGTBFL::FaceCamera(this, GetSprite());
	}
}

void AGTCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void AGTCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	IDirectionalSpriteInterface::Sprites.Remove(this);
}



//SPRITES AND COSMETIC
void AGTCharacter::InitMaterials()
{
	//BodyDMI = GetSprite()->CreateDynamicMaterialInstance(0);
	//BodyDMI->SetTextureParameterValue(TEXT("Texture"), CharacterData->BodyAsset->Image);
	//FLinearColor skinColor = UGTBFL::HSLtoRGB(CharacterData->SkinColorHSL);
	//BodyDMI->SetVectorParameterValue(TEXT("ColorSkin"), skinColor);

	//OriginalScale = GetSprite()->GetRelativeScale3D();
}

void AGTCharacter::ResetAttachments()
{
	UpdateFacing();
}

void AGTCharacter::UpdateFacing()
{
	float diff = FMath::Fmod((GetActorRotation().Yaw - UGameplayStatics::GetPlayerCameraManager(this, 0)->GetCameraRotation().Yaw + 360.0f), 360.0f);
	float bodyFB;
	if (diff < 90) {
		bodyFB = 2;
	}
	else if (diff < 180) {
		bodyFB = 1;
	}
	else if (diff < 270) {
		bodyFB = 0;
	}
	else {
		bodyFB = 3;
	}

	BodyDMI->SetScalarParameterValue(TEXT("FrontBack"), bodyFB);

	UGTBFL::FaceCamera(this, GetSprite());
}

FTransform AGTCharacter::GetTipTransform()
{
	//TODO
	return GetActorTransform();
}

void AGTCharacter::SetSpriteColor(FLinearColor color)
{
	GetSprite()->SetSpriteColor(color);
}



//MOVEMENT
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

	/*GetSprite()->SetFlipbook(FBIdle);
	HandSprite->SetFlipbook(FBIdle);*/
	ResetAttachments();
	SetActorLocation(ANavGrid::Instance->AlignToGrid(GetActorLocation()));
	ANavGrid::AddActorToGrid(this);
	ANavGrid::MoveDataID++;
	ACameraPawn::Instance->AttachCamera(nullptr);
	if (IsPartyCharacter())
		AGTPlayerController::Instance->MoveCompleted();

	if (ActionInProgress.Action)
	{
		StartAction();
	}
	else
		CompleteAction();
}

void AGTCharacter::OnMoveStep()
{
	UCombatManager::CheckDetection(this);
	if (MoveSteps.Num() == 1) //done
	{
		bIsMoving = false;
		SetActorLocation(MoveSteps[0].CalcPosition(MoveSteps[0].TimeToArrival));
		FinishedMoving();
		return;
	}
	else
	{
		MoveTimePassed -= MoveSteps[0].TimeToArrival;
		MoveSteps.RemoveAt(0);
		SetActorLocation(MoveSteps[0].CalcPosition(MoveTimePassed));
		SetActorRotation((MoveSteps[0].Velocity * FVector(1, 1, 0)).ToOrientationRotator());
		UpdateFacing();
		UGTBFL::FaceCamera(this, GetSprite());
	}
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

bool AGTCharacter::GetPathTo(FVector destination, FNavPath& path)
{
	//UE_LOG(LogTemp, Log, TEXT("%s GetPathTo %s"), *GetName(), *destination.ToString());
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
	path.ActCost = data->ActionCost;
	if (IsPartyCharacter())
		UGTHUDCode::Instance->UpdateDebugText(FString::Printf(TEXT("cost %f\tacts %d"), data->TotalCost, data->ActionCost), 2);

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

	if (IsPartyCharacter())
		ANavGrid::Instance->ShowPath(path.Path);

	return true;
}

void AGTCharacter::StartMoving(FNavPath path)
{
	if (path.ActCost == 0)
	{
		RemainingMove -= path.Cost;
	}
	else
	{
		RemainingMove = RemainingMove + Stats->MoveSpeed * path.ActCost - path.Cost;
		RemainingActions -= path.ActCost;

		if (RemainingActions > 200)
		{
			UE_LOG(LogTemp, Error ,TEXT("RemainingActions > 200; path.ActCost = %d\nsetting RemainingActions to 0"), path.ActCost);
			RemainingActions = 0;
		}
	}

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
	/*GetSprite()->SetFlipbook(FBWalk);
	HandSprite->SetFlipbook(FBWalk);*/
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
	float maxMove = RemainingMove + RemainingActions * Stats->MoveSpeed;

	for (int i = 0; i < MoveGrid.Num(); i++)
	{
		for (int j = 0; j < MoveGrid[i].Num(); j++)
		{

			if (!MoveGrid[i][j].Occupied && MoveGrid[i][j].TotalCost <= maxMove)
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

void AGTCharacter::TurnToFace(FVector target)
{
	//FVector direction = (target - GetActorLocation()) * FVector(1, 1, 0);

	//align character to axes or diagonals
	float ang = FMath::Atan2((target - GetActorLocation()).Y, (target - GetActorLocation()).X);
	ang = FMath::Floor(ang / PI * 4 + .5f) * (PI / 4);

	SetActorRotation(FVector(FMath::Cos(ang), FMath::Sin(ang), 0).ToOrientationRotator());
	//might not be needed because of Tick
	UpdateFacing();
	UGTBFL::FaceCamera(this, GetSprite());
}

float AGTCharacter::GetMoveSpeed()
{
	return Stats->MoveSpeed;
}




void AGTCharacter::StartLunge()
{
	if (LungeTime < 0) //not currently lunging
	{
		LungeTime = 1;
		PreLungePosition = GetActorLocation();
		//LungeDestination = PreLungePosition + GetActorForwardVector() * 50; //half a square
		LungeDestination = (ActionInProgress.Location + PreLungePosition) / 2;
		UE_LOG(LogTemp,Log, TEXT("Lunge:\n from: %s\n   to: %s\n targ: %s"), *PreLungePosition.ToString(), *LungeDestination.ToString(), *ActionInProgress.Location.ToString());
	}
	//if currently lunging, starting a new lunge may mess things up
	//shouldn't happen anyway
}

//ACTIONS
void AGTCharacter::StartAction()
{
	if (!ActionInProgress.Action) return;
	UE_LOG(LogTemp, Log, TEXT("Starting %s"), *ActionInProgress.Action->Name.ToString());
	if (RemainingActions == 0)
	{
		UE_LOG(LogTemp, Error, TEXT("RemainingActions == 0; cannot start action"));
		CompleteAction();
		return;
	}

	RemainingActions -= ActionInProgress.Action->ActionCost;
	if (RemainingActions > 200)
	{
		UE_LOG(LogTemp, Error, TEXT("RemainingActions > 200; Action Cost = %d\nsetting RemainingActions to 0"), ActionInProgress.Action->ActionCost);
		RemainingActions = 0;
	}
	RemainingMove = 0;
	//TODO: actions that include movement as part of them (eg Charge)?

	//making sure this is not still set. Will be set when the projectile is fired
	bWaitingOnProjectile = false;

	SetActorRotation(((ActionInProgress.Location - GetActorLocation()) * FVector(1, 1, 0)).ToOrientationRotator());
	UpdateFacing();
	UGTBFL::FaceCamera(this, GetSprite());
	//PlayActionAnim(ActionInProgress.Action->Anim);
	GetAnimInstance()->JumpToNode(TEXT("Action"));

	UParticleSystem** partPtr = ActionInProgress.Action->Particles.Find(TEXT("StartTip")); //start of action; tip of weapon
	if (partPtr != nullptr)
	{
		UParticleSystemComponent* psc = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), *partPtr, GetTipTransform());
	}
}

void AGTCharacter::DelayCheckDeathQueue()
{
	UCombatManager::CheckDeathQueue();
}

EActionAnim AGTCharacter::GetActionAnim()
{
	if (!ActionInProgress.Action) return EActionAnim::Attack;
	return ActionInProgress.Action->Anim;
}

void AGTCharacter::PerformAction(FActionData actionData)
{
	ActionInProgress = actionData;
	if(ActionInProgress.Action)
		StartAction();
	else
		CompleteAction();
}

void AGTCharacter::ResolveAction()
{
	UE_LOG(LogTemp, Log, TEXT("Resolving %s"), *ActionInProgress.Action->Name.ToString());
	if (ActionInProgress.Action)
	{
		ActionInProgress.Action->Resolve(this, ActionInProgress.Location);

		UParticleSystem** partPtr = ActionInProgress.Action->Particles.Find(TEXT("ResolveTargetLoc")); //resolution of action; target location
		if (partPtr != nullptr)
		{
			UParticleSystemComponent* psc = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), *partPtr, ActionInProgress.Location + FVector(0, 0, 80));
		}

		if (bWaitingOnProjectile)
		{
			bWaitingOnProjectile = false;
			CompleteAction();
		}
	}
	else
		CompleteAction();
}

void AGTCharacter::CompleteAction()
{
	if(bWaitingOnProjectile)
	{
		UE_LOG(LogTemp, Log, TEXT("Waiting on Projectile"));
		if(ActionInProgress.Action && !ActionInProgress.Action->ProjectileClass)
		{
			UE_LOG(LogTemp,Error, TEXT("Waiting on Projectile that never existed"));
		}

		return;
	}

	if(ActionInProgress.Action)
		UE_LOG(LogTemp, Log, TEXT("Completing %s"), *ActionInProgress.Action->Name.ToString());

	ActionInProgress.Action = nullptr;
	ANavGrid::Instance->GenerateMoveData(this);

	if (IsPartyCharacter())
	{
		UGTHUDCode::Instance->ShowHideCommandMenu(this);
		UGTHUDCode::Instance->RefreshCharacterVitals();
	}

	//UCombatManager::CheckDeathQueue();

	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &AGTCharacter::DelayCheckDeathQueue, 1.5f, false); //damage anims should be only 1 second, so after that
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
			StartAction();
		}
		else
			CompleteAction();

		return;
	}

	StartMoving(path);
	ActionInProgress = actionData;
}

void AGTCharacter::FireProjectile()
{
	if (ActionInProgress.Action && ActionInProgress.Action->ProjectileClass)
	{
		UE_LOG(LogTemp, Log, TEXT("Firing Projectile %s"), *ActionInProgress.Action->ProjectileClass->GetName());
		bWaitingOnProjectile = true;
		FTransform t = GetTipTransform();
		AProjectile* projectile = Cast<AProjectile>(GetWorld()->SpawnActor(ActionInProgress.Action->ProjectileClass, &t));
		projectile->Init(ActionInProgress.Action, this, ActionInProgress.Location);
	}
}

void AGTCharacter::Meditate()
{
	Stats->CurrentVitals[EVitals::Mana] += (int)((Stats->Level + 1)/2);
	if (Stats->CurrentVitals[EVitals::Mana] > Stats->MaxVitals[EVitals::Mana])
	{
		Stats->CurrentVitals[EVitals::Mana] = Stats->MaxVitals[EVitals::Mana];
	}

	RemainingActions -= 2;

	CompleteAction();
}





//STATS
int32 AGTCharacter::GetCurrentHealth() const
{
	int32* health = Stats->CurrentVitals.Find(EVitals::Health);
	if (health != nullptr)
		return *health;

	return 0;
}

int32 AGTCharacter::GetMaxHealth() const
{
	int32* health = Stats->MaxVitals.Find(EVitals::Health);
	if (health != nullptr)
		return *health;

	return 0;
}

int32 AGTCharacter::GetDefense(EDefenseType defense) const
{
	//return Stats->Defense[defense];

	int32* def = Stats->Defense.Find(defense);
	if (def == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("%s has incomplete defense map; missing %d %s"), *CharacterName.ToString(), (int)defense, *UGTBFL::DefenseToText(defense).ToString());
		for (auto& elem : Stats->Defense)
		{
			UE_LOG(LogTemp, Log, TEXT("   have %s"), *UGTBFL::DefenseToText(elem.Key).ToString());
		}
		return 0;
	}

	return *def;
}

int32 AGTCharacter::GetAccuracy(EAttackType attack) const
{
	//return Stats->Accuracy[attack];

	int32* accuracy = Stats->Accuracy.Find(attack);
	if (accuracy == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("%s has incomplete accuracy map; missing %d %s"), *CharacterName.ToString(), (int)attack, *UGTBFL::AttackToText(attack).ToString());
		for (auto& elem : Stats->Accuracy)
		{
			UE_LOG(LogTemp, Log, TEXT("   have %s"), *UGTBFL::AttackToText(elem.Key).ToString());
		}
		return 0;
	}

	return *accuracy;
}








void AGTCharacter::BeginTurn_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("\n----------\nBegin Turn %s\n----------"), *GetName());
	/*if (bDead)
	{
		UE_LOG(LogTemp, Log, TEXT("%s is dead, ending turn"), *GetName());
		EndTurn();
		return;
	}*/
	
	RemainingActions = Stats->MaxActions;
	RemainingMove = 0;

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
	ANavGrid::Instance->GenerateMoveData(this);
	ACameraPawn::Instance->SetActorLocation(GetActorLocation());
	UGTHUDCode::Instance->ShowCharacterInfo(this);
	UCombatManager::ResetDetection(this);


	if(IsPartyCharacter())
	{
		AGTPlayerController::Instance->ControlCharacter(this);
		UGTHUDCode::Instance->ShowHideCommandMenu(this);
	}


	AdvanceAI();
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

float AGTCharacter::RollInitiative()
{
	return FMath::RandRange(0, 20);
}

int32 AGTCharacter::ApplyDamage(int32 amount, EDamageType damageType, EVitals vital)
{
	amount -= Stats->Resist[damageType];
	if(amount < 0) amount = 0; //damage can never be negative
	UE_LOG(LogTemp, Log, TEXT("Apply Damage: %d"), amount);

	Stats->CurrentVitals[vital] -= amount;
	if (Stats->CurrentVitals[vital] <= 0)
	{
		Stats->CurrentVitals[vital] = 0;
		switch (vital)
		{
		case EVitals::Health:
			{
				//TODO: DEATH/DOWN stuff
				//WeaponSprite->SetVisibility(false);
				//ShieldSprite->SetVisibility(false);
			}
			break;
		default:
			break;
		}
	}


	return amount;
}

void AGTCharacter::OnHit(bool bMajor)
{
	ITargetableInterface::OnHit(bMajor);
	PlayHitAnim(bMajor);
}

void AGTCharacter::OnHoverStart()
{
	SetSpriteColor(FLinearColor(1.5f, 1.5f, .5f));
}

void AGTCharacter::OnHoverEnd()
{
	SetSpriteColor(FLinearColor(1, 1, 1));
}





bool AGTCharacter::IsSameTeam(ITargetable target)
{
	AGTCharacter* targetCharacter = Cast<AGTCharacter>(target->AsActor());
	if (!targetCharacter)
		return false;

	return targetCharacter->Team == Team;
}

TArray<class UAction*> AGTCharacter::GetAllAttacks()
{
	TArray<UAction*> attacks;

	if (DefaultMeleeAttack)
		attacks.Add(DefaultMeleeAttack);
	if (DefaultRangedAttack)
		attacks.Add(DefaultRangedAttack);

	//TODO: get weapon attacks
	//TODO: get skill attacks


	return attacks;
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
