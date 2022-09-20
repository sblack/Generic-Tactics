// Fill out your copyright notice in the Description page of Project Settings.


#include "NavGrid.h"
#include "HighlightManager.h"
#include "../Character/GTCharacter.h"
#include "../Character/GTAIController.h"
#include "../Combat/Action.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include "Components/CapsuleComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "NavigationSystem/Public/NavigationSystem.h"
#include "TileHighlight.h"

DEFINE_LOG_CATEGORY(LogNavGrid);

ANavGrid* ANavGrid::Instance;
TArray<FNodeData> ANavGrid::Fringe;
TArray<FVector> ANavGrid::Offsets;

int ANavGrid::MoveDataID;

// Sets default values
ANavGrid::ANavGrid()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ANavGrid::BeginPlay()
{
	UE_LOG(LogNavGrid, Log, TEXT("BeginPlay Grid"));
	Super::BeginPlay();

	Instance = this;
	HighlightManager = NewObject<UHighlightManager>();
	HighlightManager->Init();
	MoveDataID = 0;

	IntSpace = (int)Spacing;
	IntHalfSpace = IntSpace / 2;
	Spacing = (float)IntSpace;

	TArray<AActor*> temp;

	if(HighlightManager)
	{
		UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName("Terrain"), temp);
		for (int i = 0; i < temp.Num(); i++)
		{
			TInlineComponentArray<UPrimitiveComponent*> comps;
			temp[i]->GetComponents(comps);
			for (int j = 0; j < comps.Num(); j++)
			{
				UMaterialInstanceDynamic* dmi = comps[j]->CreateDynamicMaterialInstance(0);
				dmi->SetTextureParameterValue(FName("Highlight"), HighlightManager->HighlightTexture);
			}
		}
		temp.Empty();
	}

	ScanWorld();

	UGameplayStatics::GetAllActorsWithInterface(GetWorld(), UTargetableInterface::StaticClass(), temp);
	for (int i = 0; i < temp.Num(); i++)
	{
		temp[i]->SetActorLocation(AlignToGrid(temp[i]->GetActorLocation()));
		SetGridData(FGridData(temp[i]->GetActorLocation(), ITargetableInterface::Targetable(temp[i])));
	}
}

void ANavGrid::ScanWorld()
{
	FBox box = UNavigationSystemV1::GetNavigationSystem(this)->GetWorldBounds();
	UE_LOG(LogNavGrid, Log, TEXT("World %s to %s"), *box.Min.ToCompactString(), *box.Max.ToCompactString());

	int minX = XYToIndex(box.Min.X);
	int minY = XYToIndex(box.Min.Y);
	int maxX = XYToIndex(box.Max.X);
	int maxY = XYToIndex(box.Max.Y);

	for (int ix = minX; ix <= maxX; ix++)
	{
		for (int iy = minY; iy <= maxY; iy++)
		{
			ScanTile(ix, iy);
		}
	}

	//shows grid info
	//for (int i = minY; i <= maxY; i++)
	//{
	//	//FString str = FString::FromInt((int)(Grid[0][i].RegionType));
	//	FString str = FString::SanitizeFloat(Grid[minX][i].Location.Z);
	//	for (int j = minX + 1; j <= maxX; j++)
	//	{
	//		//str += FString::FromInt((int)(Grid[j][i].RegionType));
	//		str += " " + FString::SanitizeFloat(Grid[j][i].Location.Z);
	//	}
	//	UE_LOG(LogNavGrid, Log, TEXT("%s"), *str);
	//}

	Offsets.Empty(8);

	Offsets.Add(FVector(Spacing, 0, 0));
	Offsets.Add(FVector(Spacing, Spacing, 0));
	Offsets.Add(FVector(0, Spacing, 0));
	Offsets.Add(FVector(-Spacing, Spacing, 0));
	Offsets.Add(FVector(-Spacing, 0, 0));
	Offsets.Add(FVector(-Spacing, -Spacing, 0));
	Offsets.Add(FVector(0, -Spacing, 0));
	Offsets.Add(FVector(Spacing, -Spacing, 0));
}

void ANavGrid::ScanTile(int ix, int iy)
{
	FHitResult hit;
	FVector vec(IndexToXY(ix), IndexToXY(iy), 0);
	FCollisionObjectQueryParams coqp;
	coqp.AddObjectTypesToQuery(ECC_WorldStatic);

	TArray<FHitResult> hits;
	if (!GetWorld()->LineTraceMultiByObjectType(hits, vec + FVector(0, 0, 1000), vec - FVector(0, 0, 1000), FCollisionObjectQueryParams(ECC_WorldStatic)))
	{
		Grid[ix][iy] = FGridData(vec, ERegionTypes::Void);
	}
	else
	{
		ERegionTypes rt = ERegionTypes::Void;
		FVector loc = vec;
		for (int i = hits.Num() - 1; i > -1; i--)
		{
			ANavRegion* region = Cast<ANavRegion>(hits[i].Actor);
			if (region)
				rt = region->RegionType;

			if (hits[i].Actor->ActorHasTag(TEXT("Terrain")))
			{
				loc = hits[i].ImpactPoint;
				if (rt == ERegionTypes::Void)
					rt = ERegionTypes::None;
			}
		}

		Grid[ix][iy] = FGridData(loc, rt);
		//UE_LOG(LogNavGrid, Log, TEXT("%d %d %f"), ix, iy, loc.Z);

		if (rt == ERegionTypes::Start
			&& HighlightManager)
		{
			HighlightManager->SetTileColor(loc, UHighlightManager::TargetColor);
		}
	}

	//DrawDebugLine(GetWorld(), vec, vec + FVector(0, 0, 200), FColor::Cyan, false, 5.f, 0, 5.f);
}

void ANavGrid::AddNeighbors(FNodeData data, float maxCost, class AGTCharacter* character)
{
	for (int i = 0; i < 8; i++)
	{
		int d = (i + data.Dir + 1) % 8;

		FVector vec = data.Location + Offsets[d];

		FGridData gridData = GetGridData(vec);

		if (gridData.Location.Z - data.Location.Z > 50 || gridData.Location.Z - data.Location.Z < -75)
			continue;

		if (gridData.Blocked(character))
			continue;

		// A B
		// B C
		// can't move directly from A to C if either B is obstructed
		if (d % 2 == 1)
		{
			FGridData corner = GetGridData(data.Location + Offsets[d - 1]);
			if (corner.bStaticObstruction || (corner.Location.Z - data.Location.Z > 50))
				continue;
			corner = GetGridData(data.Location + Offsets[(d + 1) % 8]);
			if (corner.bStaticObstruction || (corner.Location.Z - data.Location.Z > 50))
				continue;
		}

		float cost = GetCost(vec, Offsets[d], character);
		float value = data.TotalCost + cost;

		if (value > maxCost)
			continue;

		vec.Z = gridData.Location.Z;
		FNodeData node = FNodeData();
		node.Location = AlignToGrid(vec);
		node.Origin = data.Location;
		node.Cost = cost;
		node.TotalCost = value;
		node.Dir = d;
		node.Occupied = (gridData.Occupant != nullptr);
		FringeInsert(node);
	}
}

void ANavGrid::FringeInsert(FNodeData data)
{
	if (Fringe.Num() == 0 || data.TotalCost >= Fringe[Fringe.Num() - 1].TotalCost)
	{
		Fringe.Emplace(data);
		return;
	}
	float value = data.TotalCost + ((data.Occupied) ? .01f : (float)0);

	if (value < Fringe[0].TotalCost)
	{
		//UE_LOG(LogNavGrid, Log, TEXT("First in Fringe: %f < %f"), data.Value, Fringe[0].Value);
		Fringe.EmplaceAt(0, data);
		return;
	}
	int low = 0;
	int high = Fringe.Num() - 1;
	while (high - low > 1)
	{
		int mid = (high + low) / 2;
		if (value == Fringe[mid].TotalCost)
		{
			Fringe.EmplaceAt(mid, data);
			return;
		}
		else if (value < Fringe[mid].TotalCost)
			high = mid;
		else
			low = mid;
	}
	Fringe.EmplaceAt(high, data);
}

FGridData* ANavGrid::GetGridDataPtr(FVector location)
{
	int ix = XYToIndex(location.X);
	if (ix < 0 || ix > 31)
	{
		//UE_LOG(LogNavGrid, Error, TEXT("Invalid location: %s"), *location.ToString());
		return nullptr;
	}
	int iy = XYToIndex(location.Y);
	if (iy < 0 || iy > 31)
	{
		//UE_LOG(LogNavGrid, Error, TEXT("Invalid location: %s"), *location.ToString());
		return nullptr;
	}

	//UE_LOG(LogNavGrid, Log, TEXT("Getting data for location: %s index: %d"), *location.ToString(), ix + iy*Width);
	return &(Grid[ix][iy]);
}

// Called every frame
void ANavGrid::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (HighlightManager && HighlightManager->bHasTextureUpdate)
	{
		HighlightManager->UpdateTexture();
	}
}

void ANavGrid::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UE_LOG(LogNavGrid, Log, TEXT("EndPlay Grid"));
	if (Instance == this)
		Instance = nullptr;

	Super::EndPlay(EndPlayReason);
}

FVector ANavGrid::AlignToGrid(FVector vector)
{
	return FVector(
		IndexToXY(XYToIndex(vector.X)),
		IndexToXY(XYToIndex(vector.Y)),
		vector.Z);
}

void ANavGrid::AddActorToGrid(TScriptInterface<ITargetableInterface> actor)
{
	if (!Instance)
	{
		UE_LOG(LogNavGrid, Error, TEXT("Missing Grid"));
		return;
	}

	FGridData data = Instance->GetGridData(actor->AsActor()->GetActorLocation());
	data.Occupant = actor;
	Instance->SetGridData(data);

	UE_LOG(LogNavGrid, Log, TEXT("Added %s at %s"), *actor->AsActor()->GetName(), *data.Location.ToCompactString());
}

void ANavGrid::RemoveActorFromGrid(TScriptInterface<ITargetableInterface> actor)
{
	if (!Instance)
	{
		UE_LOG(LogNavGrid, Error, TEXT("Missing Grid"));
		return;
	}

	FGridData data = Instance->GetGridData(actor->AsActor()->GetActorLocation());
	if (data.Occupant != actor)
	{
		UE_LOG(LogNavGrid, Error, TEXT("%s is not in grid at expected location"), *actor->AsActor()->GetName());
		return;
	}
	data.Occupant = nullptr;
	Instance->SetGridData(data);

	UE_LOG(LogNavGrid, Log, TEXT("Removed %s from %s"), *actor->AsActor()->GetName(), *data.Location.ToCompactString());

}

float ANavGrid::GetCost(FVector target, FVector direction, const class AGTCharacter* character)
{
	float mult;
	FGridData datum = GetGridData(target);
	/*switch (datum.RegionType)
	{
	default:
		mult = 1;
		break;
	}*/
	mult = 1;

	if (direction.X != 0 && direction.Y != 0)
		return 1.5f * mult;
	else
		return mult;
}

float ANavGrid::GetDistance(FVector vector)
{
	int x = vector.X / IntSpace; if (x < 0) x = -x;
	int y = vector.Y / IntSpace; if (y < 0) y = -y;

	float result;
	if (x > y)
	{
		result = (float)x + (float)y / 2;
	}
	else
	{
		result = (float)x / 2 + (float)y;
	}

	//UE_LOG(LogNavGrid, Log, TEXT("%s -> %f"), *vector.ToString(), result);
	return result;
}

void ANavGrid::GenerateMoveData(class AGTCharacter* character)
{
	if (character->MoveDataID == MoveDataID)
		return;

	//clock_t startTime = clock();

	character->MoveDataID = MoveDataID;
	for (int i = 0; i < character->MoveGrid.Num(); i++)
	{
		character->MoveGrid[i].Empty();
	}
	character->MoveGrid.Empty();
	Fringe.Empty();

	float maxCost = character->CurrentAP;

	FVector start = character->GetActorLocation() - FVector(0, 0, character->GetCapsuleComponent()->GetScaledCapsuleHalfHeight());
	UE_LOG(LogNavGrid, Log, TEXT("GMD: starting from %s"), *start.ToString());
	Fringe.Add(FNodeData(AlignToGrid(start)));

	//foreach node, until Fringe is empty
	while (Fringe.Num() != 0)
	{
		FNodeData data = Fringe[0];
		//...remove node and any duplicates from Fringe
		for (int i = 0; i < Fringe.Num(); i++)
		{
			if (Fringe[i].Location.X == data.Location.X && Fringe[i].Location.Y == data.Location.Y)
			{
				Fringe.RemoveAt(i);
				i--;
			}
		}

		int dX = XYToIndex(data.Location.X);
		int dY = XYToIndex(data.Location.Y);
		bool bAdded = false;

		bool bFoundX = false;
		//rows in MoveGrid are not necessarily consecutive Xs, so find row with same X value
		for (int i = 0; i < character->MoveGrid.Num(); i++)
		{
			if (character->MoveGrid[i].Num() == 0)
			{
				character->MoveGrid.RemoveAtSwap(i);
				i--;
				continue;
			}

			if (XYToIndex(character->MoveGrid[i][0].Location.X) == dX)
			{
				bFoundX = true;
				bool bFoundY = false;
				for (int j = 0; j < character->MoveGrid[i].Num(); j++)
				{
					if (XYToIndex(character->MoveGrid[i][j].Location.Y) == dY)
					{
						if (character->MoveGrid[i][j].TotalCost > data.TotalCost)
						{
							character->MoveGrid[i][j] = data;
							bAdded = true;
						}
						bFoundY = true;
						break;
					}
				}
				if (!bFoundY)
				{
					character->MoveGrid[i].Add(data);
					bAdded = true;
				}
				break;
			}
		}
		if (!bFoundX)
		{
			TArray<FNodeData> temp;
			temp.Add(data);
			character->MoveGrid.Add(temp);
			bAdded = true;
		}

		if (bAdded)
			AddNeighbors(data, maxCost, character);
	}

	/*for (int i = 0; i < controller->MoveGrid.Num(); i++)
	{
		for (int j = 0; j < controller->MoveGrid[i].Num(); j++)
		{
			UE_LOG(LogNavGrid, Log, TEXT("%s"), *controller->MoveGrid[i][j].Location.ToCompactString());
		}
	}*/

	//double time = (double)(clock() - startTime) / CLOCKS_PER_SEC * 1000.0;
	//UE_LOG(LogNavGrid, Log, TEXT("Generated MoveData: %f"), time);

	/*int count = 0;
	for (int i = 0; i < controller->MoveGrid.Num(); i++)
	{
	count += controller->MoveGrid[i].Num();

	for (int j = 0; j < controller->MoveGrid[i].Num(); j++)
	{
	DrawDebugLine(GetWorld(), controller->MoveGrid[i][j].Origin, controller->MoveGrid[i][j].Location, FColor::Red, false, 2);
	}
	}

	UE_LOG(LogNavGrid, Log, TEXT("Generated MoveData for %s: %d %d nodes"), *controller->GetName(), controller->MoveGrid.Num(), count);*/
}

void ANavGrid::ShowMoveRange(class AGTCharacter* character)
{
	if (character)
	{
		ShowMoveRange(nullptr);

		float maxCost = 5;// controller->GetTacticsCharacter()->CurrentAP;

		GenerateMoveData(character);
		for (int i = 0; i < character->MoveGrid.Num(); i++)
		{
			for (int j = 0; j < character->MoveGrid[i].Num(); j++)
			{

				//UE_LOG(LogNavGrid, Log, TEXT(" %d %d %f"), XYToIndex(controller->MoveGrid[i][j].Location.X), XYToIndex(controller->MoveGrid[i][j].Location.Y), controller->MoveGrid[i][j].Value);
				if (character->MoveGrid[i][j].TotalCost > maxCost || character->MoveGrid[i][j].Occupied)
				{
					//UE_LOG(LogNavGrid, Error, TEXT("Here it is"));
					continue;
				}

				FGridData* gridData = GetGridDataPtr(character->MoveGrid[i][j].Location);

				if (gridData == nullptr)
				{
					UE_LOG(LogNavGrid, Error, TEXT("Failed to get GridData pointer"));
					return;
				}

				

				HighlightManager->SetTileColor(gridData->Location, UHighlightManager::ReachColor);
			}
		}
	}
	else
	{
		//GEngine->AddOnScreenDebugMessage(-1, 1, FColor::Green, TEXT("Clearing Highlights"));
		UE_LOG(LogNavGrid, Log, TEXT("Clearing Highlights"));

		HighlightManager->ClearTileColors();
		PathPoints.Empty();
	}
}

void ANavGrid::ShowPath(TArray<FVector>& path)
{
	for (int i = 0; i < PathPoints.Num(); i++)
	{
		HighlightManager->SetTileColor(PathPoints[i], UHighlightManager::ReachColor);
	}
	PathPoints = path;

	for (int i = 0; i < PathPoints.Num(); i++)
	{
		HighlightManager->SetTileColor(PathPoints[i], UHighlightManager::PathColor);
	}
}

void ANavGrid::ClearPath()
{
	for (int i = 0; i < PathPoints.Num(); i++)
	{
		HighlightManager->SetTileColor(PathPoints[i], UHighlightManager::ReachColor);
	}
	PathPoints.Empty();
}

FGridData ANavGrid::GetGridData(FVector location)
{
	int ix = XYToIndex(location.X);
	if (ix < 0 || ix > 31)
	{
		//UE_LOG(LogNavGrid, Error, TEXT("Invalid location: %s"), *location.ToString());
		return FGridData(location);
	}
	int iy = XYToIndex(location.Y);
	if (iy < 0 || iy > 31)
	{
		//UE_LOG(LogNavGrid, Error, TEXT("Invalid location: %s"), *location.ToString());
		return FGridData(location);
	}

	//UE_LOG(LogNavGrid, Log, TEXT("Getting data for location: %s index: %d"), *location.ToString(), ix + iy*Width);
	return Grid[ix][iy];
}

FVector ANavGrid::GetGridLocation(FVector location)
{
	int ix = XYToIndex(location.X);
	if (ix < 0 || ix > 31)
	{
		//UE_LOG(LogNavGrid, Error, TEXT("Invalid location: %s"), *location.ToString());
		return location;
	}
	int iy = XYToIndex(location.Y);
	if (iy < 0 || iy > 31)
	{
		//UE_LOG(LogNavGrid, Error, TEXT("Invalid location: %s"), *location.ToString());
		return location;
	}

	//UE_LOG(LogNavGrid, Log, TEXT("Getting data for location: %s index: %d"), *location.ToString(), ix + iy*Width);
	return Grid[ix][iy].Location;
}

void ANavGrid::SetGridData(FGridData data)
{
	int ix = XYToIndex(data.Location.X);
	if (ix < 0 || ix > 31)
	{
		//UE_LOG(LogNavGrid, Error, TEXT("Invalid location: %s"), *location.ToString());
		return;
	}
	int iy = XYToIndex(data.Location.Y);
	if (iy < 0 || iy > 31)
	{
		//UE_LOG(LogNavGrid, Error, TEXT("Invalid location: %s"), *location.ToString());
		return;
	}

	data.Location = Grid[ix][iy].Location;
	//UE_LOG(LogNavGrid, Log, TEXT("Setting data for location: %s index: %d"), *location.ToString(), ix + iy*Width);
	Grid[ix][iy] = data;
}

TArray<FVector> ANavGrid::AdjacentLocations(FVector location)
{
	location = AlignToGrid(location);
	TArray<FVector> result;
	for (int i = 0; i < 8; i++)
	{
		FVector vec = location + Offsets[i];

		FGridData gridData = GetGridData(vec);
		if (!gridData.bStaticObstruction && gridData.RegionType != ERegionTypes::Void)
			result.Emplace(vec);
	}

	return result;
}

TArray<FVector> ANavGrid::GetWithinDistance(FVector center, float distance, float minDistance /*= 0*/)
{
	center = AlignToGrid(center);
	int ix = XYToIndex(center.X);
	int iy = XYToIndex(center.Y);

	TArray<FVector> result;
	for (int x = -distance; x < distance + 1; x++)
	{
		if (ix + x < 0 || ix + x > 31)
			continue;
		int ax = (x < 0) ? -x : x;

		for (int y = -distance; y < distance + 1; y++)
		{
			if (iy + y < 0 || iy + y > 31)
				continue;
			int ay = (y < 0) ? -y : y;

			if (ay > ax)
			{
				if (ay + ax / 2 <= distance && ay + ax / 2 >= minDistance)
					result.Add(Grid[ix + x][iy + y].Location);
			}
			else
			{
				if (ax + ay / 2 <= distance && ax + ay / 2 >= minDistance)
					result.Add(Grid[ix + x][iy + y].Location);
			}
		}
	}

	return result;
}

void ANavGrid::ShowTiles(TArray<FVector> locations, FLinearColor color)
{
	ShowMoveRange(nullptr);
	for (int i = 0; i < locations.Num(); i++)
	{
		HighlightManager->SetTileColor(locations[i], color);
	}
}

void ANavGrid::ShowTargeting(FVector source, float range)
{
	TArray<FVector> targetable = GetWithinDistance(source, range);

	ShowMoveRange(nullptr);
	for (int i = 0; i < targetable.Num(); i++)
	{
		HighlightManager->SetTileColor(targetable[i], UHighlightManager::TargetColor);
	}
}

void ANavGrid::ShowTargetingArea(IActionSource source, FVector target, class UAction* action)
{
	ShowMoveRange(nullptr);

	TArray<FVector> targetable = GetWithinDistance(source->GetLocation(), action->Range);
	bool bValidTarget = false;
	for (int i = 0; i < targetable.Num(); i++)
	{
		HighlightManager->SetTileColor(targetable[i], UHighlightManager::ReachColor);
		if (targetable[i] == target)
			bValidTarget = true;
	}

	if (bValidTarget)
	{
		TArray<FVector> area = action->GetAffectedArea(source, target);
		for (int i = 0 ; i < area.Num(); i++)
			HighlightManager->SetTileColor(area[i], UHighlightManager::TargetColor);
	}
}

TArray<FGridData*> ANavGrid::GetStartArea()
{
	TArray<FGridData*> result;
	for (int i = 0; i < 32; i++)
	{
		for (int j = 0; j < 32; j++)
		{
			if(Grid[i][j].RegionType == ERegionTypes::Start)
				result.Add(&(Grid[i][j]));
		}
	}

	return result;
}

bool FGridData::Blocked(class AGTCharacter* character)
{
	if (bStaticObstruction)
		return true;
	if (RegionType == ERegionTypes::Void)
		return true;
	/*if (Occupant && Occupant->AsActor() != controller->GetPawn())
		return true;*/

	if (Occupant && !character->IsSameTeam(Occupant))
		return true;

	return false;
}

bool FGridData::IsDefault()
{
	if (bStaticObstruction || Occupant)
		return false;
	if (RegionType != ERegionTypes::None)
		return false;

	return true;
}