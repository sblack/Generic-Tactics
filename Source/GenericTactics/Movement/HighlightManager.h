// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "HighlightManager.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogHighlight, Log, All);

/**
 * 
 */
UCLASS()
class GENERICTACTICS_API UHighlightManager : public UObject
{
	GENERATED_BODY()
protected:
	//The size of our textures
	uint32 TextureSize = 1024;
	uint32 MapSize = 32;
	uint32 HalfWorld;

	uint32 TileSize;

	//color of each pixel
	TArray<FColor> TextureData;

	//priority low to high
	//0F -> range/reach
	//30 -> selection
	//C0 -> hover
	TArray<uint8> TileData;

	//Texture regions	
	FUpdateTextureRegion2D* textureRegions;

	float borderVals[32];
	
	int HoverMaxX, HoverMaxY, HoverMinX, HoverMinY;
	int SelectMaxX, SelectMaxY, SelectMinX, SelectMinY;

	void UpdateTile(int x, int y);

public:

	UPROPERTY()
		class UTexture2D* HighlightTexture;

	//Check to see if we have a new texture.
	UPROPERTY()
		bool bHasTextureUpdate = false;

	void Init();

	void UpdateTexture();

	void SetHovered(FVector location);
	void SetHovered(TArray<FVector> locations);
	void ClearHovered();

	void SetSelected(FVector location);
	void SetSelected(TArray<FVector> locations);
	void ClearSelected();

	void SetReach(FVector location, int value);
	void SetReach(TArray<FVector> locations, int value);
	void ClearReach();

	void ClearAll();

	virtual void FinishDestroy() override;
};
