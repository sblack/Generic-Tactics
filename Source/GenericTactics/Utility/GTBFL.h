// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
//#include "EngineMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Misc/EnumRange.h"
#include "GTBFL.generated.h"

UENUM(BlueprintType)
enum class EVitals : uint8
{
	Health,
	Stamina,
	Mana
};
ENUM_RANGE_BY_FIRST_AND_LAST(EVitals, EVitals::Health, EVitals::Mana);

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
	Typeless //if any more are added, this MUST remain the last
};
ENUM_RANGE_BY_FIRST_AND_LAST(EDamageType, EDamageType::Piercing, EDamageType::Typeless);

UENUM(BlueprintType, meta = (Bitflags, UseEnumValuesAsMaskValuesInEditor = "true"))
enum class EAttackType : uint8
{
	None	= 0 UMETA(Hidden),
	Melee	= 1 << 0,
	Range	= 1 << 1,
	Magic	= 1 << 2,
};
ENUM_RANGE_BY_FIRST_AND_LAST(EAttackType, EAttackType::Melee, EAttackType::Magic);
ENUM_CLASS_FLAGS(EAttackType);

UENUM(BlueprintType)
enum class EDefenseType : uint8
{
	None = 0 UMETA(Hidden),
	Armor = 1,
	Reflex = 2,
	Fortitude = 3,
	Will = 4,
};
ENUM_RANGE_BY_FIRST_AND_LAST(EDefenseType, EDefenseType::Armor, EDefenseType::Will);

UENUM(BlueprintType)
enum class ECharStat : uint8
{
	Str,
	Dex,
	Con,
	Int,
	Wis,
	None	//not based on any character stat
};
ENUM_RANGE_BY_FIRST_AND_LAST(ECharStat, ECharStat::Str, ECharStat::Wis);

UENUM(BlueprintType)
enum class EActionAnim : uint8
{
	Attack,
	RaiseAttack,
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

UENUM(BlueprintType)
enum class ESuccessLevel : uint8
{
	CritHit,
	Hit,
	Miss
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

	UFUNCTION(BlueprintPure, Category = "Utilities|Color")
		static FLinearColor RandomSkinHSL();

	static UEnum* Vitals();
	static UEnum* Damage();


	static FText VitalToText(EVitals vital);
	static FText DamageToText(EDamageType damage);
	static FText AttackToText(EAttackType attack);
	static FText DefenseToText(EDefenseType defense);
	static FText CharStatToText(ECharStat stat);
	
};
