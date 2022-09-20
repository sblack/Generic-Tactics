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
	UPROPERTY(EditDefaultsOnly)
		TSubclassOf<AActor> FakeCharacterClass;

	UPROPERTY(BlueprintReadWrite)
		class AActor* FakeCharActor;

	UPROPERTY(BlueprintReadWrite)
		class UCharacterButtonCode* SelectedButton;

	UPROPERTY(BlueprintReadOnly)
		TArray<class UCharacterButtonCode*> PartyButtons;

	/** in-play Character of SelectedButton */
	UPROPERTY(BlueprintReadWrite)
		class AGTCharacter* PlacedCharacter;

	UPROPERTY(BlueprintReadWrite)
		TArray<class AGTCharacter*> PlacedParty;

	UPROPERTY(BlueprintReadWrite)
		int32 Count;

	/** TODO: determine count from map */
	UPROPERTY(BlueprintReadOnly)
		int32 MaxCount = 16;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		class UTextBlock* PartyCount;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		class UButton* StartButton;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		class UTextBlock* ValidText;

	TArray<FGridData*> StartArea;

	virtual void NativeConstruct() override;

	virtual void NativeDestruct() override;

	void HoverLocation_Implementation(FVector location);

	void HoverCharacter_Implementation(class AGTCharacter* character);

	void HoverVoid_Implementation();

	void ClickLocation_Implementation(FVector location);

	void ClickCharacter_Implementation(class AGTCharacter* character);

	void ClickVoid_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void CountChanged();

	void CountChanged_Implementation();

	UFUNCTION(BlueprintImplementableEvent)
	class AGTCharacter* SpawnCharacter(FVector location, class UCharacterDataAsset* CharacterData);

	/** deploy as many party members as possible, given limits of map */
	UFUNCTION(BlueprintCallable, Category = "PreCombat")
		void AutoDeploy();

public:
	static UPreCombatUICode* Instance;

	UFUNCTION(BlueprintPure, Category = "PreCombat")
		static UPreCombatUICode* GetPreCombatUI() { return Instance; }

	UFUNCTION(BlueprintNativeEvent, Category = "PreCombat")
		void HoverLocation(FVector location);

	UFUNCTION(BlueprintNativeEvent, Category = "PreCombat")
		void HoverCharacter(class AGTCharacter* character);

	UFUNCTION(BlueprintNativeEvent, Category = "PreCombat")
		void HoverVoid();

	UFUNCTION(BlueprintNativeEvent, Category = "PreCombat")
		void ClickLocation(FVector location);

	UFUNCTION(BlueprintNativeEvent, Category = "PreCombat")
		void ClickCharacter(class AGTCharacter* character);

	UFUNCTION(BlueprintNativeEvent, Category = "PreCombat")
		void ClickVoid();
	
};
