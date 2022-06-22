// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GTHUDCode.generated.h"

/**
 * 
 */
UCLASS(ABSTRACT)
class GENERICTACTICS_API UGTHUDCode : public UUserWidget
{
	GENERATED_BODY()

protected:

	virtual void NativeConstruct() override;

	virtual void NativeDestruct() override;

public:
	static UGTHUDCode* Instance;

	UFUNCTION(BlueprintPure, Category = "UI|HUD")
		static UGTHUDCode* GetTacticsHUD() { return Instance; }

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "UI|HUD")
		void ShowCharacterInfo(class AGTCharacter* character);

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "UI|HUD")
		void ShowTargetInfo(const TScriptInterface<ITargetableInterface>& target);

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "UI|HUD")
		void HideTargetInfo();

	UFUNCTION(BlueprintImplementableEvent, Category = "UI|HUD")
		void ShowCommandsTarget(const TScriptInterface<ITargetableInterface>& target);

	UFUNCTION(BlueprintImplementableEvent, Category = "UI|HUD")
		void ShowCommandsLocation(FVector location);

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "UI|HUD")
		void HideCommands();

	UFUNCTION(BlueprintImplementableEvent, Category = "UI|HUD")
		void ShowAttackText(const FText& message);

	UFUNCTION(BlueprintImplementableEvent, Category = "UI|HUD")
		void ClearAttackText();

	UFUNCTION(BlueprintImplementableEvent, Category = "UI|HUD")
		void FlashRed();

	/** shows command options for character. If NULL, hides the command menu. */
	UFUNCTION(BlueprintImplementableEvent, Category = "UI|HUD")
		void ShowHideCommandMenu(class AGTCharacter* character);

	/** returns command menu to top level */
	UFUNCTION(BlueprintImplementableEvent, Category = "UI|HUD")
		void RestoreCommandMenu();

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "UI|HUD")
		void ShowMoveCommand();
};
