// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NavRegion.h"
#include "../Utility/ActionSourceInterface.h"
#include "../Utility/TargetableInterface.h"
#include "NavGrid.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogNavGrid, Log, All);

/**
 * stores information
 */
USTRUCT(BlueprintType)
struct GENERICTACTICS_API FGridData
{
	GENERATED_USTRUCT_BODY()
public:
	FVector Location;
	bool bStaticObstruction = false;
	float Cost = 1;
	ITargetable Occupant = nullptr;
	class ATileHighlight* Tile = nullptr;
	ERegionTypes RegionType = ERegionTypes::None;

	FGridData(): RegionType(ERegionTypes::Void) {}
	FGridData(FVector loc) : Location(loc), bStaticObstruction(true) {}
	FGridData(FVector loc, float cost) : Location(loc), Cost(cost) {}
	FGridData(FVector loc, ERegionTypes rt) : Location(loc), RegionType(rt) {}
	FGridData(FVector loc, ITargetable target) : Location(loc), Occupant(target) {}

	bool Blocked(class AGTCharacter* character);

	bool IsDefault();
};

/**
 * Used in pathfinding algorithm
 */
USTRUCT(BlueprintType)
struct GENERICTACTICS_API FNodeData
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(BlueprintReadOnly)
		FVector Location;
	/** Previous Location */
	UPROPERTY(BlueprintReadOnly)
		FVector Origin;
	/** Individual Tile */
	UPROPERTY(BlueprintReadOnly)
		float Cost;
	/** Total Cost to Tile */
	UPROPERTY(BlueprintReadOnly)
		float TotalCost;

	UPROPERTY()
		bool Occupied;

	UPROPERTY()
		uint8 Dir;

	FNodeData() {}
	FNodeData(FVector loc) : Location(loc), Origin(loc), Cost(0), TotalCost(0), Occupied(false), Dir(0) {}
	FNodeData(FVector loc, FVector ori, float c, float v, uint8 d, bool occ) : Location(loc), Origin(ori), Cost(c), TotalCost(v), Occupied(occ), Dir(d) {}

	FNodeData(const FNodeData& node) : Location(node.Location), Origin(node.Origin), Cost(node.Cost), TotalCost(node.TotalCost), Occupied(node.Occupied), Dir(node.Dir) {}

	inline bool operator>(const FVector& vec) const
	{
		if (Location.X == vec.X)
			return Location.Y > vec.Y;
		else
			return Location.X > vec.X;
	}
	inline bool operator<(const FVector& vec) const
	{
		if (Location.X == vec.X)
			return Location.Y < vec.Y;
		else
			return Location.X < vec.X;
	}
	inline bool operator<=(const FVector& vec) const { return !(*this > vec); }
	inline bool operator>=(const FVector& vec) const { return !(*this < vec); }
	inline bool operator==(const FVector& vec) const
	{
		return (Location.X == vec.X) && (Location.Y == vec.Y);
	}
};

struct FNodeDataSort
{
	bool operator()(const FNodeData& a, const FNodeData& b) const
	{
		if (a.Location.X == b.Location.X)
			return a.Location.Y > b.Location.Y;
		else
			return a.Location.X > b.Location.X;
	}
};

USTRUCT(BlueprintType)
struct FNavPath
{
	GENERATED_USTRUCT_BODY()
public:
	TArray<FVector> Path;
	float Cost = 0;

	FNavPath() {}
	FNavPath(TArray<FVector> path, float cost) : Path(path), Cost(cost) {}

	void Empty() { Path.Empty(); Cost = 0; }
};

/**
 * D&D rules: square grid, diagonals are 1.5x
 */
UCLASS()
class GENERICTACTICS_API ANavGrid : public AActor
{
	GENERATED_BODY()
protected:
	static TArray<FVector> Offsets;

	static FVector StartVector;

	TArray<FGridData> GridOLD;
	//X dimension of Grid (as Grid is 2d array in 1d)
	int32 Width;

	FGridData Grid[32][32];

	TArray<FVector> PathPoints;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ExposeOnSpawn = true, ClampMin = 100))
		float Spacing = 100.f;

	//Spacing as integer
	int IntSpace = 100;
	//half of IntSpace; used in rounding for alignment
	int IntHalfSpace = 50;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void ScanWorld();

	void ScanTile(int ix, int iy);

	void AddNeighbors(FNodeData data, float maxCost, class AGTCharacter* character);
		
	void FringeInsert(FNodeData data);

	FGridData* GetGridDataPtr(FVector location);

	int XYToIndex(float xy) { return int((xy + 1600) / 100); }
	float IndexToXY(int ind) { return -1550 + ind * 100; }

public:
	static ANavGrid* Instance;
	static TArray<FNodeData> Fringe;
	static int MoveDataID;

	UPROPERTY(BlueprintReadWrite)
		TSubclassOf<class ATileHighlight> HighlightClass;

	UPROPERTY(BlueprintReadOnly)
		class UHighlightManager* HighlightManager;

	// Sets default values for this actor's properties
	ANavGrid();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	/** Aligns X & Y to grid; does not affect Z. Use GetGridLocation if need a specific location (for comparison/equality, etc)*/
	UFUNCTION(BlueprintPure, Category = "Tactics")
		virtual FVector AlignToGrid(FVector vector);

	UFUNCTION(BlueprintCallable, Category = "Tactics")
		static void AddActorToGrid(TScriptInterface<ITargetableInterface> actor);

	UFUNCTION(BlueprintCallable, Category = "Tactics")
		static void RemoveActorFromGrid(TScriptInterface<ITargetableInterface> actor);

	float GetCost(FVector target, FVector direction, const class AGTCharacter* character);

	UFUNCTION(BlueprintPure, Category = "Tactics")
		float GetDistance(FVector vector);

	void GenerateMoveData(class AGTCharacter* character);

	/** null controller clears the highlights */
	UFUNCTION(BlueprintCallable, Category = "Tactics")
		void ShowMoveRange(class AGTCharacter* character);

	void ShowPath(TArray<FVector>& path);

	void ClearPath();

	UFUNCTION(BlueprintPure, Category = "Tactics")
		static ANavGrid* GetGrid() { return Instance; }

	UFUNCTION(BlueprintPure, Category = "Tactics")
		FGridData GetGridData(FVector location);

	UFUNCTION(BlueprintPure, Category = "Tactics")
		FVector GetGridLocation(FVector location);

	UFUNCTION(BlueprintCallable, Category = "Tactics")
		void SetGridData(FGridData data);

	TArray<FVector> AdjacentLocations(FVector location);

	UFUNCTION(BlueprintPure, Category = "Tactics")
		TArray<FVector> GetWithinDistance(FVector center, float distance, float minDistance = 0);

	UFUNCTION(BlueprintCallable, Category = "Tactics")
		void ShowTiles(TArray<FVector> locations, FLinearColor color);

	UFUNCTION(BlueprintCallable, Category = "Tactics")
		void ShowTargeting(FVector source, float range);

	void ShowTargetingArea(IActionSource source, FVector target, class UAction* action);
};
