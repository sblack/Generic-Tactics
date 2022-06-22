// Fill out your copyright notice in the Description page of Project Settings.


#include "HighlightManager.h"
#include "Rendering/Texture2DResource.h"

DEFINE_LOG_CATEGORY(LogHighlight);

FLinearColor UHighlightManager::PathColor = FLinearColor(1, 1, 0); // yellow
FLinearColor UHighlightManager::ReachColor = FLinearColor(0, .5f, 1); // cyan-blue
FLinearColor UHighlightManager::TargetColor = FLinearColor(1, 1, 0); // yellow

void UHighlightManager::Init()
{
	HighlightTexture = UTexture2D::CreateTransient(TextureSize, TextureSize);
	HighlightTexture->AddToRoot();
	HighlightTexture->UpdateResource();
	textureRegions = new FUpdateTextureRegion2D(0, 0, 0, 0, TextureSize, TextureSize);
	TextureData.Init(FColor(0, 0, 0, 0), TextureSize * TextureSize);
	TileSize = TextureSize / 32;
	TileData.Init(FColor(0, 0, 0, 0), TileSize * TileSize);

	for (int i = 0; i < 16; i++)
	{
		borderVals[i] = borderVals[31 - i] = 1 - FMath::Pow(.5f, i / 2.0f);
	}
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

void UHighlightManager::ClearTileColors()
{
	for (int x = 0; x < (int)TileSize; x++)
	{
		for (int y = 0; y < (int)TileSize; y++)
		{
			if (TileData[x + y * TileSize].A == 0) continue;
			//UE_LOG(LogHighlight, Log, TEXT("%d, %d  %s"), x, y, *TileData[x + y * TileSize].ToString());

			TileData[x + y * TileSize] = FColor(0, 0, 0, 0);
			{
				for (int i = 0; i < 32; i++)
				{
					for (int j = 0; j < 32; j++)
					{
						if (!TextureData.IsValidIndex(x * 32 + i + (y * 32 + j) * TextureSize))
						{
							UE_LOG(LogHighlight, Error, TEXT("Invalid index: %d"), x * 32 + i + (y * 32 + j) * TextureSize);
							continue;
						}
						TextureData[x * 32 + i + (y * 32 + j) * TextureSize] = FColor(0, 0, 0, 0);
					}
				}
			}

			bHasTextureUpdate = true;
		}
	}
}

void UHighlightManager::SetTileColor(FVector location, FLinearColor color)
{
	int x = (int)((location.X + 1600) / 100);
	if (x < 0 || x > 31) return;
	int y = (int)((location.Y + 1600) / 100);
	if (y < 0 || y > 31) return;

	//UE_LOG(LogHighlight, Log, TEXT("\t%d\t%f\t%d\t%f"), x, location.X, y, location.Y);

	FColor fullColor = color.Quantize();

	TileData[x + y * TileSize] = fullColor;

	for (int i = 0; i < 32; i++)
	{
		for (int j = 0; j < 32; j++)
		{
			TextureData[x * 32 + i + (y * 32 + j) * TextureSize] = fullColor.WithAlpha(fullColor.A * (1 - borderVals[i] * borderVals[j]));
		}
	}
	bHasTextureUpdate = true;
}

void UHighlightManager::FinishDestroy()
{
	UE_LOG(LogHighlight, Log, TEXT("Destroying HighlightManager"));

	Super::FinishDestroy();
}
