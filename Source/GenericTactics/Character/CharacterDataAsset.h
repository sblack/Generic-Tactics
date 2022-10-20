// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "../Appearance/AppearanceAsset.h"
#include "../Appearance/ColorQuartetBFL.h"
#include "CharacterDataAsset.generated.h"

/**
*
*/
USTRUCT(BlueprintType)
struct GENERICTACTICS_API FCharacterData
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FText Name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 ID = -1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FLinearColor SkinColorHSL;

	/** starts at 0; 255 = bald */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		uint8 HairIndex;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FLinearColor HairColorHSL;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UAppearanceAsset* BodyAsset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FColorQuartet BodyColorsHSL;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UAppearanceSetAsset* HatAsset;

	/** starts at 0; 255 = hatless */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		uint8 HatIndex;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FColorQuartet HatColorsHSL;

	/*UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class UItemWeapon* Weapon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class UItemEquipment* Shield;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class UItemEquipment* Armor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<class UItemEquipment*> Accessories;*/

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 TotalXP;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 RemainingXP;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<class UFeat*> Feats;

	FCharacterData();

	FCharacterData(class UCharacterDataAsset* data, int32 _id);

	~FCharacterData();

	void Randomize();
};

/**
 * 
 */
UCLASS(BlueprintType)
class GENERICTACTICS_API UCharacterDataAsset : public UDataAsset
{
	GENERATED_BODY()
protected:

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FText Name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 ID = -1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Appearance")
		FLinearColor SkinColorHSL;

	/** starts at 0; 255 = bald */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Appearance")
		uint8 HairIndex = 255;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Appearance")
		FLinearColor HairColorHSL;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Appearance")
		UAppearanceAsset* BodyAsset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Appearance")
		FColorQuartet BodyColorsHSL;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Appearance")
		UAppearanceSetAsset* HatAsset;

	/** starts at 0; 255 = hatless */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Appearance")
		uint8 HatIndex = 255;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Appearance")
		FColorQuartet HatColorsHSL;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
		class UItemWeapon* Weapon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
		class UItemEquipment* Shield;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
		class UItemEquipment* Armor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, EditFixedSize, Category = "Inventory")
		TArray<class UItemEquipment*> Accessories;

	UPROPERTY(BlueprintReadWrite, Category = "Stats")
		int32 TotalXP;

	UPROPERTY(BlueprintReadWrite, Category = "Stats")
		int32 RemainingXP;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
		TArray<class UFeat*> Feats;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Instanced, Category = "AI")
		TArray<class UAIObjective*> AIObjectives;
	
	UFUNCTION(BlueprintCallable, Category = "Tactics")
		static UCharacterDataAsset* FromSave(struct FCharacterData saveData);

	UFUNCTION(BlueprintCallable, Category = "Tactics")
		static UCharacterDataAsset* NewCharacter();

	/** Creates a copy of source, randomizing unset cosmetic features */
	UFUNCTION(BlueprintCallable, Category = "Tactics")
		static UCharacterDataAsset* RandomCopyCharacter(UCharacterDataAsset* source, float teamHue);

	virtual void PostLoad() override;

	UFUNCTION(BlueprintPure, Category = "Appearance")
		uint8 GetBodyIndex();

	//hat assets contain up to 10 hats
	UFUNCTION(BlueprintPure, Category = "Appearance")
		uint8 GetCombinedHatIndex();

	UFUNCTION(BlueprintPure, Category = "Stats")
		virtual uint8 GetLevel();

	UFUNCTION(BlueprintCallable, Category = "Feats")
		TArray<class UFeat*> GetAvailableFeats();

	UFUNCTION(BlueprintCallable, Category = "Feats")
		void LearnFeat(class UFeat* feat);
};
