// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NavMesh/RecastNavMesh.h"
#include "GTRecastNavMesh.generated.h"

/**
 * 
 */
UCLASS()
class GENERICTACTICS_API AGTRecastNavMesh : public ARecastNavMesh
{
	GENERATED_BODY()
protected:

	static FPathFindingResult FindPath(const FNavAgentProperties& AgentProperties, const FPathFindingQuery& Query);

public:

	AGTRecastNavMesh(const FObjectInitializer& ObjectInitializer);
};

struct GENERICTACTICS_API FTacticsPath : public FNavigationPath
{
	typedef FNavigationPath Super;

	TArray<float> Costs;

	virtual float GetCostFromIndex(int32 PathPointIndex) const override;

	virtual float GetCostFromNode(NavNodeRef PathNode) const override;

};