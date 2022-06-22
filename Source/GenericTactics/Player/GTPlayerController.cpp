// Fill out your copyright notice in the Description page of Project Settings.


#include "GTPlayerController.h"
#include <EngineGlobals.h>
#include <Runtime/Engine/Classes/Engine/Engine.h>
#include "DrawDebugHelpers.h"
#include "CameraPawn.h"
#include "../Character/GTCharacter.h"
#include "../Character/GTAIPlayer.h"
#include "../Combat/CombatManager.h"
#include "../Movement/NavGrid.h"
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

	if (Hit.bBlockingHit)
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
		}


	}
}

void AGTPlayerController::ControlCharacter(class AGTAIPlayer* aiController, class AGTCharacter* chara)
{
	ActiveCharacter = chara;
	ActiveController = aiController;
}

void AGTPlayerController::MoveCompleted()
{
	UE_LOG(LogGTPlayerController, Log, TEXT("MoveCompleted"));
	bMoving = false;
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
	bTargetMove = true;
	SelectedLocation = FVector(0, 0, -1000);
	ANavGrid::Instance->ShowMoveRange(ActiveController);
}

void AGTPlayerController::BeginTargetAction()
{
	bTargetAction = true;
	SelectedLocation = FVector(0, 0, -1000);
}

void AGTPlayerController::CancelTarget()
{
	bTargetAction = bTargetMove = false;
	ANavGrid::Instance->ShowMoveRange(nullptr);
	UGTHUDCode::Instance->RestoreCommandMenu();
}

void AGTPlayerController::UpdateFinalAP(float apCost)
{
	FinalAP = ActiveCharacter->CurrentAP - apCost;
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
		OnLeftClickUpPre();
	else
	{
		if (bMoving || !ActiveController || UCombatManager::IsPerformingActions())
			return;
		OnLeftClickUpCombat();
		return;
	}
	if (bMoving || !ActiveController)
		return;
	OnLeftClickUpCombat();

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
	if (bTargetMove)
	{
		UE_LOG(LogGTPlayerController, Log, TEXT("   TargetMove"));
		if (HoverLocation == SelectedLocation) //confirm move
		{
			if (bHavePath)// && NavPathCost <= ActiveCharacter->CurrentAP)
			{
				bMoving = true;
				ActiveCharacter->CurrentAP -= NavPathCost;
				ANavGrid::Instance->ShowMoveRange(nullptr);
				ANavGrid::RemoveActorFromGrid(ActiveController->GetTacticsCharacter());
				ACameraPawn::Instance->AttachCamera(ActiveController->GetPawn());
				ActiveCharacter->StartMoving(NavPath);
				UGTHUDCode::Instance->HideCommands();
			}
		}
		else
		{
			SelectedLocation = HoverLocation;

			NavPath.Empty();
			if (ActiveController->GetPathBack(SelectedLocation, NavPath))
			{
				bHavePath = true;
				NavPathCost = 0;
				UE_LOG(LogGTPlayerController, Log, TEXT("%s"), *NavPath[0].ToString());
				for (int i = 1; i < NavPath.Num(); i++)
				{
					FVector dir = NavPath[i] - NavPath[i - 1];
					NavPathCost += ANavGrid::Instance->GetCost(NavPath[i], dir, ActiveController);
					UE_LOG(LogGTPlayerController, Log, TEXT("%s"), *NavPath[i].ToString());
				}
				UE_LOG(LogGTPlayerController, Log, TEXT("Cost: %f"), NavPathCost);
				FinalAP = ActiveCharacter->CurrentAP - NavPathCost;
				UE_LOG(LogGTPlayerController, Log, TEXT("LCUC: Path Found"));
			}
			else
			{
				bHavePath = false;
				FinalAP = ActiveCharacter->CurrentAP;
				ANavGrid::Instance->ClearPath();
				UE_LOG(LogGTPlayerController, Log, TEXT("LCUC: No Path"));
			}
		}
	}
	else if (bTargetAction)
	{
		UE_LOG(LogGTPlayerController, Log, TEXT("   TargetAction"));
		if (HoverLocation == SelectedLocation)
		{
			UCombatManager::InitiatePreparedAction(ActiveCharacter);
		}
		else
		{
			SelectedLocation = HoverLocation;
		}
	}
}

void AGTPlayerController::MouseOverTerrain(FVector location)
{
	GEngine->AddOnScreenDebugMessage(-1, 0, FColor::Red, FString::Printf(TEXT("MouseOverTerrain %s"), *location.ToString()));
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

			if (ActiveCharacter && !bMoving && SelectedLocation.Z == -1000)
			{
				if (bTargetMove)
				{

					NavPath.Empty();
					if (ActiveController->GetPathBack(HoverLocation, NavPath))
					{
						bHavePath = true;
						NavPathCost = 0;
						UE_LOG(LogGTPlayerController, Log, TEXT("%s"), *NavPath[0].ToString());
						for (int i = 1; i < NavPath.Num(); i++)
						{
							FVector dir = NavPath[i] - NavPath[i - 1];
							NavPathCost += ANavGrid::Instance->GetCost(NavPath[i], dir, ActiveController);
							UE_LOG(LogGTPlayerController, Log, TEXT("%s"), *NavPath[i].ToString());
						}
						UE_LOG(LogGTPlayerController, Log, TEXT("Cost: %f"), NavPathCost);
						FinalAP = ActiveCharacter->CurrentAP - NavPathCost;
						UE_LOG(LogGTPlayerController, Log, TEXT("MOT: Path Found"));
					}
					else
					{
						bHavePath = false;
						FinalAP = ActiveCharacter->CurrentAP;
						ANavGrid::Instance->ClearPath();
						UE_LOG(LogGTPlayerController, Log, TEXT("MOT: No Path"));
					}
				}
				else if (bTargetAction)
				{
					UCombatManager::UpdateAreaOfEffect(ActiveCharacter->GetActorLocation(), HoverLocation);
				}
			}
		}
	}
}

void AGTPlayerController::MouseOverTarget(ITargetable target)
{
	if (target == HoverTarget)
		return;

	if (HoverTarget)
		HoverTarget->OnHoverEnd();
	HoverTarget = target;
	HoverTarget->OnHoverStart();

	UGTHUDCode::Instance->ShowTargetInfo(target);
	FVector location = target->AsActor()->GetActorLocation();

	HoverLocation = ANavGrid::Instance->AlignToGrid(location);

	if (UCombatManager::IsPreCombat())
	{
		UPreCombatUICode::Instance->HoverCharacter(Cast<AGTCharacter>(target->AsActor()));
	}
	else
	{
		if (ActiveCharacter && !bMoving && SelectedLocation.Z == -1000)
		{
			if (bTargetAction)
			{
				UCombatManager::UpdateAreaOfEffect(ActiveCharacter->GetActorLocation(), HoverLocation);
			}
		}
	}

	//GEngine->AddOnScreenDebugMessage(1, 5, FColor::Cyan, FString::Printf(TEXT("%s"), *HoverLocation.ToString()));
	UpdateHover();
}
