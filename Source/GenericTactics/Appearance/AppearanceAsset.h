// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "ColorQuartetBFL.h"
#include "AppearanceAsset.generated.h"

/**
 * 
 */
UCLASS()
class GENERICTACTICS_API UAppearanceAsset : public UDataAsset
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		class UTexture2D* Image;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		FColorQuartet DefaultColorsRGB;

	//how many DefaultColors are TeamColors (hue comes from team instead of asset)
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		uint8 TeamColors = 1;
};

/**
 *
 */
UCLASS()
class GENERICTACTICS_API UAppearanceSetAsset : public UDataAsset
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		class UTexture2D* Image;

	//if a "behind" image is needed
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		class UTexture2D* ImageB;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, EditFixedSize)
		TArray<FColorQuartet> DefaultColorsRGB;

	//how many DefaultColors are TeamColors (hue comes from team instead of asset)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, EditFixedSize)
		TArray<uint8> TeamColors;

	UAppearanceSetAsset();
};
