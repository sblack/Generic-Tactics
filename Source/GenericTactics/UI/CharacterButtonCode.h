// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CharacterButtonCode.generated.h"

/**
 * 
 */
UCLASS(ABSTRACT)
class GENERICTACTICS_API UCharacterButtonCode : public UUserWidget
{
	GENERATED_BODY()
protected:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		class USpriteWidgetCode* SpriteWidget;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		class UImage* Frame;

	/** because I can't just call it Button in the blueprint */
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		class UButton* Button0;

	UPROPERTY(BlueprintReadOnly)
		class AGTCharacter* Character;

	UPROPERTY(BlueprintReadWrite)
		bool bIsSelected;

	UPROPERTY(BlueprintReadWrite)
		bool bIsPlaced;

	/** if false, OnClicked won't be called if IsSelected==True */
	UPROPERTY(BlueprintReadWrite, EditInstanceOnly, meta = (ExposeOnSpawn))
		bool bClickIfAlreadySelected;

	void Select_Implementation();

	void Deselect_Implementation();

	void Place_Implementation(class AGTCharacter* chara);

	void Unplace_Implementation();
public:
	UPROPERTY(BlueprintReadWrite, EditInstanceOnly, meta = (ExposeOnSpawn))
		class UCharacterDataAsset* CharacterData;

	UFUNCTION(BlueprintPure)
		bool GetIsPlaced() { return bIsPlaced;}

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
		void Select();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
		void Deselect();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
		void Place(class AGTCharacter* chara);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
		void Unplace();
};
