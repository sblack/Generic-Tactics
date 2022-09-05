// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
//#include "EngineMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "GTBFL.generated.h"

UENUM(BlueprintType)
enum class EVitals : uint8
{
	Health,
	Stamina,
	Mana,
	COUNT UMETA(Hidden)
};

UENUM(BlueprintType)
enum class EDamageType : uint8
{
	Piercing,
	Crushing,
	Slashing,
	Fire,
	Acid,
	Cold,
	Electric,
	Typeless, //if any more are added, this MUST remain the last
	COUNT UMETA(Hidden)
};

UENUM(BlueprintType)
enum class EAttackType : uint8
{
	Melee,
	Range,
	Magic
};

UENUM(BlueprintType)
enum class EActionAnim : uint8
{
	Attack,
	Cast,
	Block,
	Crouch
};

UENUM(BlueprintType)
enum class EStatusAnim : uint8
{
	Idle,
	Struck,
	Down
};

/**
 * 
 */
UCLASS()
class GENERICTACTICS_API UGTBFL : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
private:
	static UEnum* VitalsPtr;
	static UEnum* DamagePtr;
public:
	UFUNCTION(BlueprintPure, Category = "Utilities|String")
		static void NewLine(FString& newLine) { newLine = "\n"; }

	/** rotates component (or actor, if null) to face camera */
	UFUNCTION(BlueprintCallable)
		static void FaceCamera(class AActor* actor, class USceneComponent* component);

	/** Make a color from individual color components (HSL space; Hue is [0..360) while Saturation and Luminance are [0..1]) */
	//UFUNCTION(BlueprintPure, Category = "Utilities|Color", meta = (DisplayName = "HSL to RGB"))
		static FLinearColor HSLtoRGB(float H, float S, float L, float A = 1);

	/** Convert HSL color to RGB color (Hue is [0..360); Saturation and Luminance are [0..1]) */
	UFUNCTION(BlueprintPure, Category = "Utilities|Color", meta = (DisplayName = "HSL to RGB"))
		static FLinearColor HSLtoRGB(FLinearColor InHSL);

	/** Breaks apart a color into individual HSL components (as well as alpha) (Hue is [0..360) while Saturation and Luminance are [0..1]) */
	//UFUNCTION(BlueprintPure, Category = "Utilities|Color", meta = (DisplayName = "RGB to HSL"))
		static void RGBToHSL(FLinearColor InColor, float& H, float& S, float& L, float& A);

	/** Convert RGB color to HSL color (Hue is [0..360); Saturation and Luminance are [0..1]) */
	UFUNCTION(BlueprintPure, Category = "Utilities|Color", meta = (DisplayName = "RGB to HSL"))
		static FLinearColor RGBToHSL(FLinearColor InRGB);

	static UEnum* Vitals();
	static UEnum* Damage();
	
};
