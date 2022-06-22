// Fill out your copyright notice in the Description page of Project Settings.


#include "GTAIController.h"
#include "GTCharacter.h"
#include "CharacterDataAsset.h"
#include "AIObjective.h"
#include "../Combat/CombatManager.h"
#include "../Player/CameraPawn.h"
#include "../UI/GTHUDCode.h"



void AGTAIController::BeginPlay()
{
	Super::BeginPlay();
	Initiative = GetTacticsCharacter()->RollInitiative();
	MoveDataID = -1;
	bIsMyTurn = false;
}

void AGTAIController::BeginTurn_Implementation()
{
	if (IsPendingKill())
	{
		return;
	}

	UE_LOG(LogTemp, Log, TEXT("Begin Turn %s"), *GetName());
	bIsMyTurn = true;
	GetTacticsCharacter()->OnBeginTurn();

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
	if (GetPawn() == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("No Pawn"));
		return;
	}

	ANavGrid::MoveDataID++;
	ANavGrid::Instance->GenerateMoveData(this);
	ACameraPawn::Instance->SetActorLocation(GetPawn()->GetActorLocation());
	UGTHUDCode::Instance->ShowCharacterInfo(GetTacticsCharacter());

	if (UseAI() && GetTacticsCharacter()->CharacterData)
	{
		bool bEndTurn = true; //if AI can't perform any objectives, end turn
		for (int i = 0; i < GetTacticsCharacter()->CharacterData->AIObjectives.Num(); i++)
		{
			if (GetTacticsCharacter()->CharacterData->AIObjectives[i]->Attempt(this))
			{
				bEndTurn = false;
				UE_LOG(LogTemp, Log, TEXT("%s successful"), *GetTacticsCharacter()->CharacterData->AIObjectives[i]->GetDebugString());
				break;
			}
			else
			{
				UE_LOG(LogTemp, Log, TEXT("%s failed"), *GetTacticsCharacter()->CharacterData->AIObjectives[i]->GetDebugString());
			}
		}

		if (bEndTurn)
		{
			UE_LOG(LogTemp, Log, TEXT("all objectives impossible, ending turn"));
			EndTurn();
		}
	}
}

void AGTAIController::EndTurn_Implementation()
{
	bIsMyTurn = false;
	GetTacticsCharacter()->OnEndTurn();

	UE_LOG(LogTemp, Log, TEXT("End Turn %s"), *GetName());
	UCombatManager::AdvanceInitiative();
}

const FNodeData* AGTAIController::FindMoveData(FVector vec) const
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

FORCEINLINE class AGTCharacter* AGTAIController::GetTacticsCharacter()
{
	return Cast<AGTCharacter>(GetCharacter());
}

//FPathFollowingRequestResult AGTAIController::MoveTo(const FAIMoveRequest& MoveRequest, FNavPathSharedPtr* OutPath /*= nullptr*/)
//{
//	UE_LOG(LogTemp, Log, TEXT("%s MoveTo"), *GetName());
//
//	FPathFollowingRequestResult result = Super::MoveTo(MoveRequest, OutPath);
//
//	if (result.Code != EPathFollowingRequestResult::Failed)
//	{
//		if (result.Code == EPathFollowingRequestResult::RequestSuccessful)
//		{
//			UE_LOG(LogTemp, Log, TEXT("Success"));
//		}
//		else
//		{
//			UE_LOG(LogTemp, Log, TEXT("Already There"));
//		}
//		const FNodeData* data = FindMoveData(MoveRequest.GetDestination());
//		if (data != nullptr)
//		{
//			GetTacticsCharacter()->CurrentAP -= data->Value;
//			//UE_LOG(LogTemp, Log, TEXT("MoveTo %f"), data->Value);
//		}
//		else
//		{
//			UE_LOG(LogTemp, Error, TEXT("MoveTo WTH"));
//		}
//	}
//	else
//	{
//		UE_LOG(LogTemp, Log, TEXT("Failed"));
//	}
//
//	return result;
//}

//void AGTAIController::OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result)
//{
//	Super::OnMoveCompleted(RequestID, Result);
//
//	if (IsPendingKill())
//	{
//		return;
//	}
//
//	if (GetGameInstance()->IsPendingKillOrUnreachable())
//	{
//		return;
//	}
//
//	GetPawn()->SetActorLocation(ANavGrid::Instance->AlignToGrid(GetPawn()->GetActorLocation()));
//	ANavGrid::AddActorToGrid(GetTacticsCharacter());
//	ANavGrid::MoveDataID++;
//	ANavGrid::Instance->GenerateMoveData(this);
//	ACameraPawn::Instance->AttachCamera(nullptr);
//
//	if (GetTacticsCharacter()->CurrentAP < 1)
//		EndTurn();
//}

bool AGTAIController::GetPathBack(FVector destination, TArray<FVector>& pathBack) const
{
	UE_LOG(LogTemp, Log, TEXT("%s GetPathBack"), *GetName());
	destination = ANavGrid::Instance->AlignToGrid(destination);
	const FNodeData* data = FindMoveData(destination);
	if (data == nullptr)
	{
		//destination may not be in movedata due to range, so no error message
		UE_LOG(LogTemp, Log, TEXT("PathBack failed: destination %s not found in MoveData"), *destination.ToCompactString());
		UE_LOG(LogTemp, Log, TEXT("MoveGrid.Num() = %d"), MoveGrid.Num());
		return false;
	}

	pathBack.Emplace(destination);

	int sanity = 0;
	while (data->Value != 0)
	{
		//UE_LOG(LogTemp, Log, TEXT("PathBack: %s %f"), *data->Location.ToCompactString(), data->Value);
		data = FindMoveData(data->Origin);
		if (data == nullptr)
		{
			UE_LOG(LogTemp, Error, TEXT("PathBack failed: origin not found in MoveData"));
			return false;
		}

		pathBack.EmplaceAt(0, data->Location);

		if (++sanity > 100)
		{
			UE_LOG(LogTemp, Error, TEXT("PathBack failed: sanity"));
			return false;
		}
	}

	return true;
}

//FAIRequestID AGTAIController::RequestMove(const FAIMoveRequest& MoveRequest, FNavPathSharedPtr Path)
//{
//	UE_LOG(LogTemp, Log, TEXT("%s RequestMove"), *GetName());
//
//	FAIRequestID result = Super::RequestMove(MoveRequest, Path);
//	if (result.IsValid())
//	{
//		ANavGrid::RemoveActorFromGrid(GetTacticsCharacter());
//		ACameraPawn::Instance->AttachCamera(GetPawn());
//	}
//
//	return result;
//}

TArray<FNodeData> AGTAIController::GetReachableArea()
{
	ANavGrid::Instance->GenerateMoveData(this);

	TArray<FNodeData> result;
	for (int i = 0; i < MoveGrid.Num(); i++)
	{
		for (int j = 0; j < MoveGrid[i].Num(); j++)
		{

			if (!MoveGrid[i][j].Occupied && MoveGrid[i][j].Value <= GetTacticsCharacter()->CurrentAP)
				result.Add(MoveGrid[i][j]);
		}
	}

	return result;
}

void AGTAIController::CheckAP()
{
	if (GetTacticsCharacter()->CurrentAP < 1)
		EndTurn();
}

bool AGTAIController::IsSameTeam(ITargetable target)
{
	AGTCharacter* targetCharacter = Cast<AGTCharacter>(target->AsActor());
	if (!targetCharacter)
		return false;

	return targetCharacter->ActorHasTag(FName("Party")) == GetPawn()->ActorHasTag(FName("Party"));
}

float AGTAIController::CostToLocation(FVector Location)
{
	const FNodeData* data = FindMoveData(ANavGrid::Instance->AlignToGrid(Location));
	if (data == nullptr)
		return -1;
	else
		return data->Value;
}

float AGTAIController::CostToAdjacent(FVector location, FVector& nearest)
{
	TArray<FVector> adj = ANavGrid::Instance->AdjacentLocations(location);
	float leastCost = 100;
	for (int i = 0; i < adj.Num(); i++)
	{
		const FNodeData* data = FindMoveData(adj[i]);
		if (data == nullptr)
			continue;
		if (data->Value < leastCost)
		{
			leastCost = data->Value;
			nearest = data->Location;
		}
	}
	if (leastCost == 100)
		leastCost = -1;

	return leastCost;
}

FVector AGTAIController::TruncatedPath(FVector destination, float limit /*= -1*/)
{
	if (limit == -1)
		limit = GetTacticsCharacter()->CurrentAP;

	destination = ANavGrid::Instance->AlignToGrid(destination);
	const FNodeData* data = FindMoveData(destination);
	if (data == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("TP: destination not found"));
		return destination;
	}
	if (data->Value <= limit && !data->Occupied)
		return destination;

	while (true)
	{
		data = FindMoveData(data->Origin);
		if (data == nullptr)
		{
			UE_LOG(LogTemp, Error, TEXT("TP: data not found"));
			return destination;
		}
		if (data->Value <= limit && !data->Occupied)
			return data->Location;
	}
}
