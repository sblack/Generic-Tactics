// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "../Utility/ActionSourceInterface.h"
#include "InitiativeTrackCode.generated.h"

/**
 * 
 */
UCLASS(ABSTRACT)
class GENERICTACTICS_API UInitiativeTrackCode : public UUserWidget
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintImplementableEvent)
		void AddMarker(const TScriptInterface<IActionSourceInterface>& actSrc);

	UFUNCTION(BlueprintImplementableEvent)
		void AdvanceInitiative();

	UFUNCTION(BlueprintImplementableEvent)
		void RemoveMarker(const TScriptInterface<IActionSourceInterface>& actSrc);
};
