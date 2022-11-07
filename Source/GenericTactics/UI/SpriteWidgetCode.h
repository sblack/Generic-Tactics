// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "../Appearance/ColorQuartetBFL.h" //WHAT THE FUCK
#include "SpriteWidgetCode.generated.h"

/**
 * Incomplete, currently unnecessary. Leaving in solution, in case it becomes needed later.
 */
UCLASS(ABSTRACT)
class GENERICTACTICS_API USpriteWidgetCode : public UUserWidget
{
	GENERATED_BODY()
protected:
	UPROPERTY(BlueprintReadWrite)
		class UMaterialInstanceDynamic* BodyDMI;

	UPROPERTY(BlueprintReadWrite)
		class UMaterialInstanceDynamic* HairDMI;

	UPROPERTY(BlueprintReadWrite)
		class UMaterialInstanceDynamic* HatFrontDMI;

	UPROPERTY(BlueprintReadWrite)
		class UMaterialInstanceDynamic* HatBackDMI;

	UPROPERTY(BlueprintReadWrite)
		class UMaterialInstanceDynamic* MonsterDMI;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		class UImage* Body;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		class UImage* Hair;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		class UImage* HatFront;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		class UImage* HatBack;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		class UImage* Monster;

	UPROPERTY(EditDefaultsOnly)
		class UMaterial* MonsterMaterial;

	/*virtual void NativePreConstruct() override;

	UFUNCTION(BlueprintCallable)
		void InitMats();*/

public:

	UFUNCTION(BlueprintCallable)
		void SetFromData(class UCharacterDataAsset* Data, uint8 Team);

	UFUNCTION(BlueprintCallable)
		void SetFromMonster(class AMonsterBase* Data, uint8 Team);
	
	UFUNCTION(BlueprintCallable)
		void SetSkinColor(FLinearColor Value);

	UFUNCTION(BlueprintCallable)
		void SetHairColor(FLinearColor Value);

	UFUNCTION(BlueprintCallable)
		void SetHairIndex(uint8 Index);

	UFUNCTION(BlueprintCallable)
		void SetBodyType(class UTexture2D* Image);

	UFUNCTION(BlueprintCallable)
		void SetBodyColors(struct FColorQuartet Colors);

	UFUNCTION(BlueprintCallable)
		void SetHatType(uint8 Index, class UTexture2D* Image, class UTexture2D* ImageB);

	UFUNCTION(BlueprintCallable)
		void SetHatColors(struct FColorQuartet Colors);
};
