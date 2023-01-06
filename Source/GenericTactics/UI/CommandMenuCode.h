// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CommandMenuCode.generated.h"

/**
 * 
 */
UCLASS(ABSTRACT)
class GENERICTACTICS_API UCommandMenuCode : public UUserWidget
{
	GENERATED_BODY()
private:
	TArray<class UAction*> SubActions;
	
	int SubMenuOffset = 0;
	/** is last button of submenu the next button */
	bool bNextButton = false;

	/** text of label when not hovering over a button */
	FText LabelTextUnhovered;

	void DeselectMain();

	void UpdateSubMenu();

protected:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		class UBorder* Main;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		class UButton* MoveButton;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		class UButton* AttackButton;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		class UButton* TechButton;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		class UButton* MagicButton;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		class UButton* ModalButton;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		class UButton* ItemsButton;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		class UButton* InfoButton;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		class UButton* EndButton;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		class UBorder* Sub;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		class UBorder* Label;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		class UTextBlock* LabelText;

	UPROPERTY(BlueprintReadOnly)
		TArray<class UButton*> SubButtons;

	UPROPERTY(BlueprintReadOnly)
		TArray<class UImage*> SubIcons;

	UPROPERTY(BlueprintReadOnly)
		class UTexture2D* NextIcon;

	UPROPERTY(BlueprintReadOnly)
		class UButton* SelectedMain = nullptr;

	UPROPERTY(BlueprintReadOnly)
		class UButton* SelectedSub = nullptr;

	UFUNCTION(BlueprintCallable)
		void ButtonUnhovered();

	UFUNCTION(BlueprintCallable)
		void MoveClicked();

	UFUNCTION(BlueprintCallable)
		void AttackClicked();


	UFUNCTION(BlueprintCallable)
		void BindButtons();

	void SubClicked(int32 index);

	void SubHovered(int32 index);
};
