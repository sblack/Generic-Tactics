// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Item.generated.h"

/**
 * 
 */
UCLASS()
class GENERICTACTICS_API UItem : public UDataAsset
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
		FText Name;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
		FText Description;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		class UTexture2D* Image;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		FLinearColor Color0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		FLinearColor Color1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		FLinearColor Color2;

	UPROPERTY(BlueprintReadWrite, Transient)
		class UUserWidget* ItemSlotWidget;

	UFUNCTION(BlueprintPure)
		virtual FText GetItemText();
	
};

USTRUCT(BlueprintType)
struct GENERICTACTICS_API FInventoryEntry
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UItem* Item = nullptr;

	//total number
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = 0))
		int32 Count;

	//number of item currently equipped. Only applies to equipment
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = 0))
		int32 Equipped;

	FInventoryEntry() : Item(NULL), Count(0), Equipped(0) {}
	FInventoryEntry(UItem* item, int32 count = 1) : Item(item), Count(count), Equipped(0) {}
	~FInventoryEntry() { Item = nullptr; }
};