// Fill out your copyright notice in the Description page of Project Settings.


#include "GTRecastNavMesh.h"
#include "../Character/GTAIController.h"
#include "NavGrid.h"

FPathFindingResult AGTRecastNavMesh::FindPath(const FNavAgentProperties& AgentProperties, const FPathFindingQuery& Query)
{
	return ENavigationQueryResult::Error;
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