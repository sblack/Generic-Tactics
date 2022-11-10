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

	uint32 TileSize;

	//color of each pixel
	TArray<FColor> TextureData;

	//color of each tile
	TArray<FColor> TileData;

	//Texture regions	
	FUpdateTextureRegion2D* textureRegions;

	float borderVals[32];

public:

	UPROPERTY()
		class UTexture2D* HighlightTexture;

	//Check to see if we have a new texture.
	UPROPERTY()
		bool bHasTextureUpdate = false;

	void Init();

	void UpdateTexture();

	UFUNCTION(BlueprintCallable, Category = "Tactics|Tile")
		void ClearTileColors();

	UFUNCTION(BlueprintCallable, Category = "Tactics|Tile")
		void SetTileColor(FVector location, FLinearColor color);

	virtual void FinishDestroy() override;
};
