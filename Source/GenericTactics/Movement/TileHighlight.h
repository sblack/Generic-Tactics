// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DecalActor.h"
#include "NavGrid.h"
#include "TileHighlight.generated.h"

/**
 * 
 */
UCLASS(ABSTRACT, meta = (HideDropdown))
class GENERICTACTICS_API ATileHighlight : public ADecalActor
{
	GENERATED_BODY()
protected:
	static UMaterial* Material;
	static float Spacing;

	struct FGridData* GridData;

	UFUNCTION(BlueprintPure, Category = "Internal")
		struct FGridData GetGridData() { return *GridData; }

	UFUNCTION(BlueprintPure, Category = "Internal")
		static UMaterial* GetTileMaterial() { return Material; }

	UFUNCTION(BlueprintPure, Category = "Internal")
		static float GetTileSpacing() { return Spacing; }

public:
	UFUNCTION(BlueprintImplementableEvent, Category = "Tile")
		void ShowReachable();

	UFUNCTION(BlueprintImplementableEvent, Category = "Tile")
		void ShowPath();

	void SetLocation(struct FGridData* gridData);

	UFUNCTION(BlueprintCallable, Category = "Tile")
		static void SetTileMaterial(UMaterial* material) { Material = material; }

	UFUNCTION(BlueprintCallable, Category = "Tile")
		static void SetTileSpacing(float meters) { Spacing = meters; }

	UFUNCTION(BlueprintImplementableEvent, Category = "Tile")
		void ShowColor(FLinearColor color);
};
