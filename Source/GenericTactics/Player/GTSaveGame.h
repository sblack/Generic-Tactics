// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "../Character/CharacterDataAsset.h"
#include "../Items/Item.h"
#include "GTSaveGame.generated.h"

/**
 * 
 */
UCLASS()
class GENERICTACTICS_API UGTSaveGame : public USaveGame
{
	GENERATED_BODY()
public:
	static UGTSaveGame* Instance;

	UPROPERTY(BlueprintReadWrite)
		TArray<struct FCharacterData> Characters;

	UPROPERTY(Transient)
		TMap<int32, class UCharacterDataAsset*> AssetCache;

	UPROPERTY(BlueprintReadWrite)
		TArray<struct FInventoryEntry> Inventory;

	UGTSaveGame();

	UFUNCTION(BlueprintPure)
		static UGTSaveGame* GetSaveGame() { return Instance; }

	/** Saves changes in asset to struct. Does not save to file. */
	UFUNCTION(BlueprintCallable, Category = "Save")
		void SaveCharacter(class UCharacterDataAsset* character);

	/** Saves changes in assets to structs. Does not save to file. */
	/*UFUNCTION(BlueprintCallable, Category = "Save")
		void SaveCharacters();*/

	UFUNCTION(BlueprintCallable, Category = "Save")
		void DeleteCharacter(class UCharacterDataAsset* character);

	UFUNCTION(BlueprintCallable)
		class UCharacterDataAsset* GetCharacterByID(int32 ID);

	UFUNCTION(BlueprintCallable)
		TArray<class UCharacterDataAsset*> CharacterAssets();
};
