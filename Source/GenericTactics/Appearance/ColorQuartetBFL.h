// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "ColorQuartetBFL.generated.h"


/**
*
*/
USTRUCT(BlueprintType)
struct GENERICTACTICS_API FColorQuartet
{
	GENERATED_USTRUCT_BODY()
public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FLinearColor Color0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FLinearColor Color1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FLinearColor Color2;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FLinearColor Color3;

	/**
	 * Copies HSL Quartet, replacing the hue values according to TeamColors.
	 * @param Hue New hue value [0..360)
	 * @param TeamColors Number of colors to change; eg 1 is only Color0, 4+ is all
	 */
	FColorQuartet ChangeTeamHue(float Hue, int TeamColors);
};

/**
 * 
 */
UCLASS()
class GENERICTACTICS_API UColorQuartetBFL : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintPure, Category = "Appearance")
		static FLinearColor GetQuartetMember(struct FColorQuartet Target, uint8 index);

	UFUNCTION(BlueprintCallable, Category = "Appearance")
		static FLinearColor SetQuartetMember(UPARAM(ref) struct FColorQuartet& Target, uint8 index, FLinearColor Color);

	UFUNCTION(BlueprintCallable, Category = "Rendering|Material")
		static void SetColorQuartetParameter(class UMaterialInstanceDynamic* Target, struct FColorQuartet Colors);

	UFUNCTION(BlueprintPure, Category = "Appearance")
		static FColorQuartet HSLtoRGBQuartet(FColorQuartet Target);
};
