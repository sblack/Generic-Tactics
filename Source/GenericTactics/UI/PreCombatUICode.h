// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PreCombatUICode.generated.h"

/**
 * UI for choosing party members before starting combat
 */
UCLASS(ABSTRACT)
class GENERICTACTICS_API UPreCombatUICode : public UUserWidget
{
	GENERATED_BODY()
protected:
	virtual void NativeConstruct() override;

	virtual void NativeDestruct() override;

public:
	static UPreCombatUICode* Instance;

	UFUNCTION(BlueprintPure, Category = "PreCombat")
		static UPreCombatUICode* GetPreCombatUI() { return Instance; }

	UFUNCTION(BlueprintImplementableEvent, Category = "PreCombat")
		void HoverLocation(FVector location);

	UFUNCTION(BlueprintImplementableEvent, Category = "PreCombat")
		void HoverCharacter(class AGTCharacter* character);

	UFUNCTION(BlueprintImplementableEvent, Category = "PreCombat")
		void HoverVoid();

	UFUNCTION(BlueprintImplementableEvent, Category = "PreCombat")
		void ClickLocation(FVector location);

	UFUNCTION(BlueprintImplementableEvent, Category = "PreCombat")
		void ClickCharacter(class AGTCharacter* character);

	UFUNCTION(BlueprintImplementableEvent, Category = "PreCombat")
		void ClickVoid();
	
};
