// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SpriteWidgetCode.generated.h"

/**
 * Incomplete, currently unnecessary. Leaving in solution, in case it becomes needed later.
 */
UCLASS(ABSTRACT)
class GENERICTACTICS_API USpriteWidgetCode : public UUserWidget
{
	GENERATED_BODY()
protected:
	UPROPERTY(BlueprintReadOnly)
		class UMaterialInstanceDynamic* BodyDMI;

	UPROPERTY(BlueprintReadOnly)
		class UMaterialInstanceDynamic* HairDMI;

	UPROPERTY(BlueprintReadOnly)
		class UMaterialInstanceDynamic* HatFrontDMI;

	UPROPERTY(BlueprintReadOnly)
		class UMaterialInstanceDynamic* HatBackDMI;

public:

	UFUNCTION(BlueprintCallable)
		void SetFromData(class UCharacterDataAsset* Data, uint8 Team);
	
	UFUNCTION(BlueprintCallable)
		void SetSkinColor(FLinearColor Value);
};
