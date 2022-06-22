// Fill out your copyright notice in the Description page of Project Settings.


#include "GTRecastNavMesh.h"
#include "../Character/GTAIController.h"
#include "NavGrid.h"

FPathFindingResult AGTRecastNavMesh::FindPath(const FNavAgentProperties& AgentProperties, const FPathFindingQuery& Query)
{
	UE_LOG(LogTemp, Log, TEXT("GTRecastNavMesh::FindPath"));
	// wrangle a 'this' of the correct type
	const ANavigationData* Self = Query.NavData.Get();
	const AGTRecastNavMesh* Custom = dynamic_cast<const AGTRecastNavMesh*>(Self);
	check(Custom != nullptr);

	if (Custom == nullptr)
	{
		return ENavigationQueryResult::Error;
	}

	if (ANavGrid::Instance == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("Missing Grid"));
		return ENavigationQueryResult::Error;
	}

	FPathFindingResult Result(ENavigationQueryResult::Error);

	// create a new path if we need to, or reuse an existing one
	Result.Path = Query.PathInstanceToFill.IsValid() ? Query.PathInstanceToFill : Self->CreatePathInstance<FTacticsPath>(Query);

	FTacticsPath* NavPath = (FTacticsPath*)(Result.Path.Get());

	if (NavPath != nullptr)
	{
		if ((Query.StartLocation - Query.EndLocation).IsNearlyZero())
		{
			Result.Path->GetPathPoints().Reset();
			Result.Path->GetPathPoints().Add(FNavPathPoint(Query.EndLocation));
			Result.Result = ENavigationQueryResult::Success;
		}
		else if (Query.QueryFilter.IsValid())
		{
			UE_LOG(LogTemp, Log, TEXT("FindPath: %s"), *Query.Owner->GetName());

			TArray<FVector> pathBack;
			const AGTAIController* controller = Cast<AGTAIController>(Query.Owner.Get());
			if (!controller->GetPathBack(Query.EndLocation, pathBack))
			{
				return ENavigationQueryResult::Error;
			}

			UE_LOG(LogTemp, Log, TEXT("PathBack complete"));

			FVector dir = FVector::ZeroVector;
			float cost = 0;
			for (int i = 1; i < pathBack.Num(); i++)
			{
				FVector nDir = pathBack[i] - pathBack[i - 1];
				if (nDir != dir)
				{
					dir = nDir;
					Result.Path->GetPathPoints().Add(FNavPathPoint(pathBack[i - 1]));
					if (i > 1)
					{
						NavPath->Costs.Add(cost);
						cost = 0;
					}
				}
				cost += ANavGrid::Instance->GetCost(pathBack[i], nDir, controller);
			}

			Result.Path->GetPathPoints().Add(FNavPathPoint(pathBack[pathBack.Num() - 1]));
			NavPath->Costs.Add(cost);

			UE_LOG(LogTemp, Log, TEXT("NavPath complete"));

			// run your pathfinding algorithm from Query.StartLocation to Query.EndLocation here,
			// adding each point on the path to NavPath->GetPathPoints()
			// the path must contain at least 2 points that aren't the start location

			// if your algorithm can only find a partial path call NavPath->SetIsPartial(true)
			// and remember to check if Query.bAllowPartialPaths is set
			// if it isn't, you should return ENavigationQueryResult::Fail

			// and finally, remember to call this!
			NavPath->MarkReady();

			Result.Result = ENavigationQueryResult::Success;
		}
	}

	return Result;
}

AGTRecastNavMesh::AGTRecastNavMesh(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	FindPathImplementation = FindPath;
	FindHierarchicalPathImplementation = FindPath;
}

float FTacticsPath::GetCostFromIndex(int32 PathPointIndex) const
{
	const uint32 PathPointsCount = Costs.Num();
	float PathCost = 0.f;

	for (uint32 PathIndex = PathPointIndex; PathIndex < PathPointsCount; ++PathIndex)
	{
		PathCost += Costs[PathIndex];
	}

	return PathCost;
}

float FTacticsPath::GetCostFromNode(NavNodeRef PathNode) const
{
	UE_LOG(LogTemp, Error, TEXT("Need GetCostFromNode after all"));
	return 0.0f;
}