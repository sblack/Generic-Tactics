// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CombatLogCode.generated.h"

/**
 * 
 */
UCLASS(ABSTRACT)
class GENERICTACTICS_API UCombatLogCode : public UUserWidget
{
	GENERATED_BODY()
protected:

	UPROPERTY(EditAnywhere)
		int MaxMessages = 10;

	TArray<FText> Messages;

	int MessageCount = 0;
	int TopMessage = 0;

	UPROPERTY(BlueprintReadWrite)
		class URichTextBlock* TextBlock;

	virtual void NativeConstruct() override;

	virtual void NativeDestruct() override;

public:
	static UCombatLogCode* Instance;

	UFUNCTION(BlueprintPure, Category = "UI")
		UCombatLogCode* GetCombatLog() { return Instance; }

	UFUNCTION(BlueprintCallable, Category = "UI")
		void LogMessage(FText text);
	
};
