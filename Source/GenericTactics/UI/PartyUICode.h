// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "../Character/CharacterDataAsset.h"
#include "PartyUICode.generated.h"

/**
 * 
 */
UCLASS(ABSTRACT)
class GENERICTACTICS_API UPartyUICode : public UUserWidget
{
	GENERATED_BODY()
protected:
	virtual void NativeConstruct() override;

	virtual void NativeDestruct() override;

	virtual void NativePreConstruct() override;
public:
	static UPartyUICode* Instance;

	UPROPERTY(BlueprintReadWrite)
		class UCharacterDataAsset* DisplayedCharacter;

	UFUNCTION(BlueprintPure, Category = "UI|Party")
		static UPartyUICode* GetPartyUI() { return Instance; }

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "UI|Party")
		void SelectCharacter(class UCharacterDataAsset* character);

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "UI|Party")
		void SetCharacterName(const FText& name);

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "UI|Party")
		void ClearCharacterName();

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "UI|Party")
		void ShowFeatConfirm(const class UFeat* feat);

	/** bUpdateFeatPage - if true, update the feat lists */
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "UI|Party")
		void HideFeatConfirm(bool bUpdateFeatPage);
};

/**
*
*/
UCLASS(ABSTRACT)
class GENERICTACTICS_API UPartyUIPanelCode : public UUserWidget
{
	GENERATED_BODY()
public:
	static FLinearColor PickedColor;
	static bool bHavePickedColor;

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "UI|Party")
		void UpdatePanel();

	//Cancel whatever might be in progress
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "UI|Party")
		void Cancel();

	UFUNCTION(BlueprintPure, Category = "UI|Party")
		static bool HavePickedColor() { return bHavePickedColor; }

	UFUNCTION(BlueprintPure, Category = "UI|Party")
		static FLinearColor GetPickedColor() { return PickedColor; }

	UFUNCTION(BlueprintCallable, Category = "UI|Party")
		static void SetPickedColor(FLinearColor color) { bHavePickedColor = true; PickedColor = color; }
};