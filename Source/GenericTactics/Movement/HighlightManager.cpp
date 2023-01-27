// Fill out your copyright notice in the Description page of Project Settings.


#include "HighlightManager.h"
#include "Rendering/Texture2DResource.h"

DEFINE_LOG_CATEGORY(LogHighlight);

void UHighlightManager::UpdateTile(int x, int y)
{
	FColor color = FColor::Transparent;
	uint8 tile = TileData[x + y * MapSize];
	if (tile & 0xC0)
		color = FColor::Yellow;
	else if (tile & 0x30)
		color = FColor::Green;
	else if (tile == 1)
		color = FColor::Cyan;
	else if (tile != 0)
		color = FColor::FColor(0, tile % 2 * 127, 255);

	for (int i = 0; i < (int)TileSize; i++)
	{
		for (int j = 0; j < (int)TileSize; j++)
		{
			TextureData[x * TileSize + i + (y * TileSize + j) * TextureSize] = color.WithAlpha(color.A * (1 - borderVals[i] * borderVals[j]));
		}
	}
	bHasTextureUpdate = true;
}

void UHighlightManager::Init()
{
	HighlightTexture = UTexture2D::CreateTransient(TextureSize, TextureSize);
	HighlightTexture->AddToRoot();
	HighlightTexture->UpdateResource();
	textureRegions = new FUpdateTextureRegion2D(0, 0, 0, 0, TextureSize, TextureSize);
	TextureData.Init(FColor(0, 0, 0, 0), TextureSize * TextureSize);
	TileSize = TextureSize / MapSize;
	TileData.Init(0, MapSize * MapSize);
	HalfWorld = MapSize / 2 * 100;

	for (int i = 0; i < (int)TileSize / 2; i++)
	{
		borderVals[i] = borderVals[TileSize - 1 - i] = 1 - FMath::Pow(.5f, i / 2.0f);
	}

	HoverMaxX = HoverMaxY = -1;
	HoverMinX = HoverMinY = TileSize;
	SelectMaxX = SelectMaxY = -1;
	SelectMinX = SelectMinY = TileSize;

}

void UHighlightManager::UpdateTexture()
{
	if (HighlightTexture == nullptr)
	{
		if (!HighlightTexture)
		{
			UE_LOG(LogHighlight, Error, TEXT("yes, they are indeed different"));
		}
		UE_LOG(LogHighlight, Error, TEXT("HighlightTexture doesn't exist? Postponing UpdateTexture()"));
		return;
	}
	bHasTextureUpdate = false;
	HighlightTexture->UpdateResource(); // WHAT IS THE PROBLEM HERE?
	HighlightTexture->UpdateTextureRegions((int32)0, (uint32)1, textureRegions, (uint32)(4 * TextureSize), (uint32)4, (uint8*)TextureData.GetData());
}

void UHighlightManager::SetHovered(FVector location)
{
	int x = (int)((location.X + HalfWorld) / 100);
	if (x < 0 || x >(int)MapSize - 1) return;
	int y = (int)((location.Y + HalfWorld) / 100);
	if (y < 0 || y >(int)MapSize - 1) return;

	if (x < HoverMinX) HoverMinX = x;
	if (x > HoverMaxX) HoverMaxX = x;
	if (y < HoverMinY) HoverMinY = y;
	if (y > HoverMaxY) HoverMaxY = y;

	TileData[x + y * MapSize] = TileData[x + y * MapSize] | 0x40;
	UpdateTile(x, y);
}

void UHighlightManager::SetHovered(TArray<FVector> locations)
{
	for (int i = 0 ; i < locations.Num(); i++)
		SetHovered(locations[i]);
}

void UHighlightManager::ClearHovered()
{
	for (int x = HoverMinX; x < HoverMaxX + 1; x++)
	{
		for (int y = HoverMinY; y < HoverMaxY + 1; y++)
		{
			TileData[x + y * MapSize] = TileData[x + y * MapSize] & 0x3F;
			UpdateTile(x, y);
		}
	}

	HoverMaxX = HoverMaxY = -1;
	HoverMinX = HoverMinY = TileSize;
}

void UHighlightManager::SetSelected(FVector location)
{
	int x = (int)((location.X + HalfWorld) / 100);
	if (x < 0 || x >(int)MapSize - 1) return;
	int y = (int)((location.Y + HalfWorld) / 100);
	if (y < 0 || y >(int)MapSize - 1) return;

	if (x < SelectMinX) SelectMinX = x;
	if (x > SelectMaxX) SelectMaxX = x;
	if (y < SelectMinY) SelectMinY = y;
	if (y > SelectMaxY) SelectMaxY = y;

	TileData[x + y * MapSize] = TileData[x + y * MapSize] | 0x10;
	UpdateTile(x, y);
}

void UHighlightManager::SetSelected(TArray<FVector> locations)
{
	for (int i = 0; i < locations.Num(); i++)
		SetSelected(locations[i]);
}

void UHighlightManager::ClearSelected()
{
	for (int x = SelectMinX; x < SelectMaxX + 1; x++)
	{
		for (int y = SelectMinY; y < SelectMaxY + 1; y++)
		{
			TileData[x + y * MapSize] = TileData[x + y * MapSize] & 0xCF;
			UpdateTile(x, y);
		}
	}

	SelectMaxX = SelectMaxY = -1;
	SelectMinX = SelectMinY = TileSize;
}

void UHighlightManager::SetReach(FVector location, int value)
{
	int x = (int)((location.X + HalfWorld) / 100);
	if (x < 0 || x >(int)MapSize - 1) return;
	int y = (int)((location.Y + HalfWorld) / 100);
	if (y < 0 || y >(int)MapSize - 1) return;

	if (value < 0 || value > 14) value = 0; //clear
	else value = value + 1;

	TileData[x + y * MapSize] = (TileData[x + y * MapSize] & 0xF0) + value;
	UpdateTile(x, y);
}

void UHighlightManager::SetReach(TArray<FVector> locations, int value)
{
	if (value < 0 || value > 14) value = 0; //clear
	else value = value + 1;

	for (int i = 0; i < locations.Num(); i++)
	{
		int x = (int)((locations[i].X + HalfWorld) / 100);
		if (x < 0 || x >(int)MapSize - 1) return;
		int y = (int)((locations[i].Y + HalfWorld) / 100);
		if (y < 0 || y >(int)MapSize - 1) return;

		TileData[x + y * MapSize] = (TileData[x + y * MapSize] & 0xF0) + value;
		UpdateTile(x, y);
	}
}

void UHighlightManager::ClearReach()
{
	for (int x = 0; x < (int)MapSize; x++)
	{
		for (int y = 0; y < (int)MapSize; y++)
		{
			TileData[x + y * MapSize] = TileData[x + y * MapSize] & 0xF0;
			UpdateTile(x, y);
		}
	}
}

void UHighlightManager::ClearAll()
{
	TextureData.Init(FColor(0, 0, 0, 0), TextureSize * TextureSize);
	TileData.Init(0, MapSize * MapSize);
	HoverMaxX = HoverMaxY = -1;
	HoverMinX = HoverMinY = TileSize;
	SelectMaxX = SelectMaxY = -1;
	SelectMinX = SelectMinY = TileSize;

	bHasTextureUpdate = true;
}

void UHighlightManager::FinishDestroy()
{
	UE_LOG(LogHighlight, Log, TEXT("Destroying HighlightManager"));

	Super::FinishDestroy();
}
