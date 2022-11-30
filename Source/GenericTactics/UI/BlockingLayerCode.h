// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BlockingLayerCode.generated.h"

/**
 * transparent layer in front of menus, to block or restrict input when necessary
 */
UCLASS(ABSTRACT)
class GENERICTACTICS_API UBlockingLayerCode : public UUserWidget
{
	GENERATED_BODY()
protected:
	/** Border widget that parents the rest of the BlockingLayer */
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		class UBorder* BaseBorder;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		class UWidgetSwitcher* Switcher;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		class UTextBlock* TextBlock;

	virtual void NativeConstruct() override;

	virtual void NativeDestruct() override;
public:
	static UBlockingLayerCode* Instance;

	UFUNCTION(BlueprintPure)
		static UBlockingLayerCode* GetBlockingLayer() { return Instance; }

	UFUNCTION(BlueprintCallable)
		void StopBlocking();

	UFUNCTION(BlueprintCallable)
		void BlockWithText(FText text);

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
		void ShowFeatConfirm(const class UFeat* feat);

	/** bUpdateFeatPage - if true, update the feat lists */
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
		void HideFeatConfirm(bool bUpdateFeatPage);
};
