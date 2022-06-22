// Fill out your copyright notice in the Description page of Project Settings.


#include "GTAIPlayer.h"
#include "GTCharacter.h"
#include "../Movement/NavGrid.h"
#include "../Player/GTPlayerController.h"
#include "../UI/GTHUDCode.h"

void AGTAIPlayer::BeginTurn_Implementation()
{
	if (IsPendingKill())
	{
		return;
	}
	Super::BeginTurn_Implementation();
	AGTPlayerController::Instance->ControlCharacter(this, GetTacticsCharacter());
	ANavGrid::Instance->ShowMoveRange(this);
	UGTHUDCode::Instance->ShowHideCommandMenu(GetTacticsCharacter());
}

void AGTAIPlayer::EndTurn_Implementation()
{
	ANavGrid::Instance->ShowMoveRange(nullptr);
	UGTHUDCode::Instance->ShowHideCommandMenu(nullptr);

	Super::EndTurn_Implementation();
}

//void AGTAIPlayer::OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result)
//{
//	Super::OnMoveCompleted(RequestID, Result);
//
//	if (IsPendingKill())
//	{
//		return;
//	}
//
//	AGTPlayerController::Instance->MoveCompleted();
//
//	if (bIsMyTurn)
//		ANavGrid::Instance->ShowMoveRange(this);
//}

bool AGTAIPlayer::GetPathBack(FVector destination, TArray<FVector>& pathBack) const
{
	bool result = Super::GetPathBack(destination, pathBack);
	if (result)
	{
		ANavGrid::Instance->ShowPath(pathBack);
	}

	return result;
}

void AGTAIPlayer::FinishedMoving()
{
	Super::FinishedMoving();

	if (IsPendingKill())
	{
		return;
	}

	AGTPlayerController::Instance->MoveCompleted();

	if (bIsMyTurn)
		ANavGrid::Instance->ShowMoveRange(this);
}
