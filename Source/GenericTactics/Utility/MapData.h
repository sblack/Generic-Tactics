// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine/DataTable.h"
#include "CoreMinimal.h"
#include "MapData.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct FMapData : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		FName MapName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		FText DisplayName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		FText Description;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		class UTexture2D* Image;

	FMapData(){}
};
