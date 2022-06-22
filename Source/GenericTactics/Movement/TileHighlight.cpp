// Fill out your copyright notice in the Description page of Project Settings.


#include "TileHighlight.h"

UMaterial* ATileHighlight::Material;

float ATileHighlight::Spacing = 1;

void ATileHighlight::SetLocation(struct FGridData* gridData)
{
	if (gridData != nullptr)
	{
		if (GridData != nullptr && GridData->Tile == this)
		{
			GridData->Tile = nullptr;
		}

		GridData = gridData;
		if (GridData->Tile != nullptr && GridData->Tile != this)
		{
			UE_LOG(LogTemp, Warning, TEXT("Tile already assigned to location %s"), *gridData->Location.ToString());

			GridData->Tile->SetLocation(nullptr);

			//GridData = nullptr;
			//SetActorHiddenInGame(true);
			//return;
		}
		GridData->Tile = this;

		SetActorLocation(GridData->Location);
		SetActorHiddenInGame(false);
	}
	else if (GridData != nullptr)
	{
		if (GridData->Tile == this)
		{
			GridData->Tile = nullptr;
		}
		GridData = nullptr;

		SetActorHiddenInGame(true);
	}
}