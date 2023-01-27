// Fill out your copyright notice in the Description page of Project Settings.


#include "GTPlayerController.h"
#include <EngineGlobals.h>
#include <Runtime/Engine/Classes/Engine/Engine.h>
#include "DrawDebugHelpers.h"
#include "CameraPawn.h"
#include "../Character/GTCharacter.h"
#include "../Character/GTAIPlayer.h"
#include "../Combat/CombatManager.h"
#include "../UI/GTHUDCode.h"
#include "../UI/PreCombatUICode.h"
//#include "../Utility/TacticsCheatManager.h"

DEFINE_LOG_CATEGORY(LogGTPlayerController);

AGTPlayerController* AGTPlayerController::Instance;

void AGTPlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);

	if (UCombatManager::IsPerformingActions())
		return;

	FHitResult Hit;
	GetHitResultUnderCursorByChannel(UEngineTypes::ConvertToTraceType(ECC_Camera), false, Hit);

	if (Hit.bBlockingHit && !bUIBlockHover)
	{
		if (Hit.Actor == nullptr)
		{
			if (Hit.Component != nullptr && Hit.Component->ComponentHasTag(FName("Terrain")))
			{
				MouseOverTerrain(Hit.ImpactPoint);
			}
		}
		else
		{
			//GEngine->AddOnScreenDebugMessage(-1, 0.1f, FColor::Cyan, FString::Printf(TEXT("Blocked by %s"), *Hit.GetActor()->GetName()));
			if (Hit.Actor->ActorHasTag(FName("Terrain")))
			{
				MouseOverTerrain(Hit.ImpactPoint);
			}
			else if (Hit.Actor->GetClass()->ImplementsInterface(UTargetableInterface::StaticClass()))
			{
				MouseOverTarget(ITargetableInterface::Targetable(Hit.GetActor()));
			}
		}
	}
	else
	{
		if (!isnan(HoverLocation.X)) //so it doesn't happen every tick
		{
			if (HoverTarget)
			{
				HoverTarget->OnHoverEnd();
				HoverTarget = nullptr;
			}
			HoverLocation.X = NAN;

			if (UCombatManager::IsPreCombat())
			{
				UPreCombatUICode::Instance->HoverVoid();
			}
			else
			{
				UGTHUDCode::Instance->HideTargetInfo();
			}
		}


	}
}

void AGTPlayerController::ControlCharacter(class AGTCharacter* chara)
{
	ActiveCharacter = chara;
}

void AGTPlayerController::MoveCompleted()
{
	UE_LOG(LogGTPlayerController, Log, TEXT("MoveCompleted"));
	CancelTarget();
}

FString AGTPlayerController::GetHoverText()
{
	if (Instance->HoverTarget)
	{
		return Instance->HoverTarget->AsActor()->GetName();
	}
	else
	{
		return Instance->HoverLocation.ToString();
	}
}

void AGTPlayerController::BeginTargetMove()
{
	PCState = EPCState::TargetMove;
	SelectedLocation = FVector(0, 0, -1000);
	ANavGrid::Instance->ShowMoveRange(ActiveCharacter);
}

void AGTPlayerController::BeginTargetAction(class UAction* action)
{
	if (!action)
	{
		UE_LOG(LogGTPlayerController, Error, TEXT("BeginTargetAction requires an Action"));
		return;
	}
	PCState = EPCState::TargetAction;
	SelectedLocation = FVector(0, 0, -1000);
	SelectedAction = action;
	//ANavGrid::Instance->ShowTargetingArea(ActiveCharacter, SelectedLocation, SelectedAction);
	ANavGrid::Instance->ShowTargetableArea(ActiveCharacter, SelectedAction);
}

void AGTPlayerController::CancelTarget()
{
	PCState = EPCState::Idle;
	ANavGrid::Instance->ShowMoveRange(nullptr);
	UGTHUDCode::Instance->RestoreCommandMenu();
}

AGTPlayerController::AGTPlayerController()
{
	bShowMouseCursor = true;
	bEnableMouseOverEvents = true;
	DefaultMouseCursor = EMouseCursor::Crosshairs;
	//CheatClass = UTacticsCheatManager::StaticClass();
}

void AGTPlayerController::SetupInputComponent()
{
	// set up gameplay key bindings
	Super::SetupInputComponent();

	InputComponent->BindAxis("MoveForward", this, &AGTPlayerController::MoveForward).bExecuteWhenPaused = true;
	InputComponent->BindAxis("MoveRight", this, &AGTPlayerController::MoveRight).bExecuteWhenPaused = true;
	InputComponent->BindAxis("RotateCamera", this, &AGTPlayerController::RotateCamera).bExecuteWhenPaused = true;

	InputComponent->BindAxis("PrepMove");// .bExecuteWhenPaused = true;

										 //binding a key crashes the BP preview, apparently
	if (GetWorld()->WorldType != EWorldType::EditorPreview)
	{
		InputComponent->BindKey(EKeys::LeftMouseButton, IE_Released, this, &AGTPlayerController::OnLeftClickUp);// .bExecuteWhenPaused = true;

		InputComponent->BindAxisKey(EKeys::MouseWheelAxis, this, &AGTPlayerController::Zoom).bExecuteWhenPaused = true;
	}
}

void AGTPlayerController::BeginPlay()
{
	UE_LOG(LogGTPlayerController, Log, TEXT("BeginPlay PlayerController"));
	Super::BeginPlay();

	Instance = this;

	FInputModeGameAndUI InputMode;
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	InputMode.SetHideCursorDuringCapture(true);
	SetInputMode(InputMode);

	//CheatManager = NewObject<UTacticsCheatManager>(this, UTacticsCheatManager::StaticClass());
}

void AGTPlayerController::MoveForward(float value)
{
	ACameraPawn::Instance->MoveForward(value);
}

void AGTPlayerController::MoveRight(float value)
{
	ACameraPawn::Instance->MoveRight(value);
}

void AGTPlayerController::RotateCamera(float value)
{
	ACameraPawn::Instance->RotateCamera(value);
}

void AGTPlayerController::Zoom(float value)
{
	ACameraPawn::Instance->ZoomIn(value);
}

void AGTPlayerController::OnLeftClickUp()
{
	UE_LOG(LogGTPlayerController, Log, TEXT("OnLeftClickUp"));

	if (UCombatManager::IsPreCombat())
	{
		OnLeftClickUpPre();
	}
	else
	{
		if (!ActiveCharacter || PCState == EPCState::Active)
			return;
		OnLeftClickUpCombat();
	}
}

void AGTPlayerController::OnLeftClickUpPre()
{
	UE_LOG(LogGTPlayerController, Log, TEXT("OnLeftClickUpPre"));
	if (HoverTarget)
		UPreCombatUICode::Instance->ClickCharacter(Cast<AGTCharacter>(HoverTarget->AsActor()));
	else
	{
		if (isnan(HoverLocation.X))
		{
			UE_LOG(LogGTPlayerController, Log, TEXT("X is NaN"));
			UPreCombatUICode::Instance->ClickVoid();
			return;
		}

		FGridData data = ANavGrid::Instance->GetGridData(HoverLocation);
		if (data.RegionType == ERegionTypes::Start)
			UPreCombatUICode::Instance->ClickLocation(data.Location);
		else
			UPreCombatUICode::Instance->ClickVoid();
	}
}

void AGTPlayerController::OnLeftClickUpCombat()
{
	UE_LOG(LogGTPlayerController, Log, TEXT("OnLeftClickUpCombat"));
	if (PCState == EPCState::TargetMove)
	{
		UE_LOG(LogGTPlayerController, Log, TEXT("   TargetMove"));
		if (HoverLocation == SelectedLocation) //confirm move
		{
			if (bHavePath)// && NavPathCost <= ActiveCharacter->CurrentAP)
			{
				PCState = EPCState::Active;
				ANavGrid::Instance->ShowMoveRange(nullptr);
				ActiveCharacter->StartMoving(NavPath);
				UGTHUDCode::Instance->HideCommands();
			}
		}
		else
		{
			SelectedLocation = HoverLocation;

			NavPath.Empty();
			if (ActiveCharacter->GetPathTo(SelectedLocation, NavPath))
			{
				bHavePath = true;
				FinalRemainingActions = ActiveCharacter->RemainingActions - NavPath.ActCost;
				//UE_LOG(LogGTPlayerController, Log, TEXT("LCUC: Path Found"));
			}
			else
			{
				bHavePath = false;
				FinalRemainingActions = ActiveCharacter->RemainingActions;
				ANavGrid::Instance->ClearPath();
				//UE_LOG(LogGTPlayerController, Log, TEXT("LCUC: No Path"));
			}
		}
	}
	else if (PCState == EPCState::TargetAction)
	{
		UE_LOG(LogGTPlayerController, Log, TEXT("   TargetAction"));
		if (HoverLocation == SelectedLocation) //confirm
		{
			//UCombatManager::InitiatePreparedAction(ActiveCharacter);
			if (ANavGrid::Instance->GetWithinDistance(ActiveCharacter->GetActorLocation(), SelectedAction->Range).Contains(HoverLocation))
			{
				GEngine->AddOnScreenDebugMessage(-1, 1, FColor::Cyan, FString::Printf(TEXT("VALID TARGET")));
				FActionData data;
				data.Action = SelectedAction;
				data.Source = ActiveCharacter;
				data.Location = SelectedLocation;
				ActiveCharacter->PerformAction(data);
				PCState = EPCState::Active;
				ANavGrid::Instance->ShowMoveRange(nullptr);
				UGTHUDCode::Instance->HideCommands();
			}
			else
			{
				GEngine->AddOnScreenDebugMessage(-1, 1, FColor::Red, FString::Printf(TEXT("INVALID TARGET")));
			}
		}
		else //select area for confirmation
		{
			//TODO: check whether Action CAN target self
			if(HoverLocation != ActiveCharacter->GetActorLocation())
			{
				SelectedLocation = HoverLocation;
				ANavGrid::Instance->ShowTargetingArea(ActiveCharacter, SelectedLocation, SelectedAction, true);
			}
		}
	}
}

void AGTPlayerController::MouseOverTerrain(FVector location)
{
	GEngine->AddOnScreenDebugMessage(-1, 0, FColor::Red, FString::Printf(TEXT("MouseOverTerrain %s"), *location.ToString()));

	if(UGTHUDCode::Instance)
	{
		if (HoverTarget)
			UGTHUDCode::Instance->UpdateDebugText(FString::Printf(TEXT("HoverTarget %s\nHoverLocation %s"), *HoverTarget->GetDisplayName().ToString(), *HoverLocation.ToString()));
		else
			UGTHUDCode::Instance->UpdateDebugText(FString::Printf(TEXT("HoverTarget NULL\nHoverLocation %s"), *HoverLocation.ToString()));
	}
	//location = ANavGrid::Instance->AlignToGrid(location);
	location = ANavGrid::Instance->GetGridLocation(location);
	GEngine->AddOnScreenDebugMessage(-1, 0, FColor::Red, FString::Printf(TEXT("%s vs %s"), *location.ToString(), *HoverLocation.ToString()));

	if (FVector2D(location) == FVector2D(HoverLocation))
		return;

	GEngine->AddOnScreenDebugMessage(-1, .1f, FColor::Red, FString::Printf(TEXT("Location != HoverLocation")));
	HoverLocation = location;


	FGridData data = ANavGrid::Instance->GetGridData(location);
	if (data.Occupant != nullptr)
	{
		GEngine->AddOnScreenDebugMessage(-1, 0, FColor::Red, FString::Printf(TEXT("Occupant != null; MouseOverTarget")));
		MouseOverTarget(data.Occupant);
	}
	else
	{
		UpdateHover();

		if (HoverTarget)
			HoverTarget->OnHoverEnd();

		HoverTarget = nullptr;
		UGTHUDCode::Instance->HideTargetInfo();

		if (UCombatManager::IsPreCombat())
		{
			//FGridData data = ATacticsGrid::Instance->GetGridData(HoverLocation);
			data = ANavGrid::Instance->GetGridData(HoverLocation);
			if (data.RegionType == ERegionTypes::Start)
				UPreCombatUICode::Instance->HoverLocation(data.Location);
			else
				UPreCombatUICode::Instance->HoverVoid();
		}
		else
		{
			if (ActiveCharacter)
			{
				GEngine->AddOnScreenDebugMessage(1, 0.1f, FColor::Cyan, FString::Printf(TEXT("%s"), *ActiveCharacter->CharacterName.ToString()));
			}
			else
			{
				GEngine->AddOnScreenDebugMessage(1, 0.1f, FColor::Red, FString::Printf(TEXT("No Active Character")));
			}

			if (bMoving)
			{
				GEngine->AddOnScreenDebugMessage(2, 0.1f, FColor::Red, FString::Printf(TEXT("Moving")));
			}
			else
			{
				GEngine->AddOnScreenDebugMessage(2, 0.1f, FColor::Cyan, FString::Printf(TEXT("Not Moving")));
			}

			if (SelectedLocation.Z == -1000)
			{
				GEngine->AddOnScreenDebugMessage(3, 0.1f, FColor::Cyan, FString::Printf(TEXT("Selected Z == -1000")));
			}
			else
			{
				GEngine->AddOnScreenDebugMessage(3, 0.1f, FColor::Red, FString::Printf(TEXT("Selected Z != -1000")));
			}

			if (ActiveCharacter && SelectedLocation.Z == -1000)
			{
				if (PCState == EPCState::TargetMove)
				{
					NavPath.Empty();
					if (ActiveCharacter->GetPathTo(HoverLocation, NavPath))
					{
						bHavePath = true;
						FinalRemainingActions = ActiveCharacter->RemainingActions - NavPath.ActCost;
						//UE_LOG(LogGTPlayerController, Log, TEXT("MOT: Path Found"));
					}
					else
					{
						bHavePath = false;
						FinalRemainingActions = ActiveCharacter->RemainingActions;
						ANavGrid::Instance->ClearPath();
						//UE_LOG(LogGTPlayerController, Log, TEXT("MOT: No Path"));
					}
				}
				else if (PCState == EPCState::TargetAction)
				{
					//UCombatManager::UpdateAreaOfEffect(ActiveCharacter->GetActorLocation(), HoverLocation);
					ANavGrid::Instance->ShowTargetingArea(ActiveCharacter, HoverLocation, SelectedAction, false);
				}
			}
		}
	}
}

void AGTPlayerController::MouseOverTarget(ITargetable target)
{
	GEngine->AddOnScreenDebugMessage(-1, 0, FColor::Red, FString::Printf(TEXT("MouseOverTarget %s"), *target->GetDisplayName().ToString()));
	if (UGTHUDCode::Instance)
		UGTHUDCode::Instance->UpdateDebugText(FString::Printf(TEXT("HoverTarget %s\nHoverLocation %s"), *target->GetDisplayName().ToString(), *HoverLocation.ToString()));
	if (target == HoverTarget)
		return;

	if (HoverTarget)
		HoverTarget->OnHoverEnd();
	HoverTarget = target;
	HoverTarget->OnHoverStart();

	UGTHUDCode::Instance->ShowTargetInfo(target);
	FVector location = target->AsActor()->GetActorLocation();

	HoverLocation = ANavGrid::Instance->GetGridLocation(location);

	if (UCombatManager::IsPreCombat())
	{
		UPreCombatUICode::Instance->HoverCharacter(Cast<AGTCharacter>(target->AsActor()));
	}
	else
	{
		if (ActiveCharacter && PCState == EPCState::TargetAction && SelectedLocation.Z == -1000)
		{
			//UCombatManager::UpdateAreaOfEffect(ActiveCharacter->GetActorLocation(), HoverLocation);
			ANavGrid::Instance->ShowTargetingArea(ActiveCharacter, HoverLocation, SelectedAction, false);
		}
	}

	//GEngine->AddOnScreenDebugMessage(1, 5, FColor::Cyan, FString::Printf(TEXT("%s"), *HoverLocation.ToString()));
	UpdateHover();
}
