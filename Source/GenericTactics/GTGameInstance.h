// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Appearance/AppearanceAsset.h"
#include "GTGameInstance.generated.h"

/**
 * 
 */
UCLASS(ABSTRACT)
class GENERICTACTICS_API UGTGameInstance : public UGameInstance
{
	GENERATED_BODY()
protected:
	UPROPERTY(EditAnywhere)
		TArray<FString> FeatPaths;

	UPROPERTY(EditAnywhere)
		TArray<FString> ItemPaths;

	void LoadData();

public:
	static UGTGameInstance* Instance;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (ClampMin = 1))
		int32 XPPerLevel = 40;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		TArray<UAppearanceAsset*> BodyTypes;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		TArray<UAppearanceSetAsset*> HatTypes;

	UPROPERTY(BlueprintReadOnly)
		TArray<class UFeat*> Feats;

	UPROPERTY(BlueprintReadOnly)
		TArray<class UItemEquipment*> Equipment;

	UPROPERTY(EditDefaultsOnly, Category = "Highlight Color")
		FLinearColor PathColor;

	UPROPERTY(EditDefaultsOnly, Category = "Highlight Color")
		FLinearColor TargetColor;

	UPROPERTY(EditDefaultsOnly, Category = "Highlight Color")
		TArray<FLinearColor> ReachColors;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (RowType = "MapData"))
		class UDataTable* MapDataTable;

	/** XP Level of recently fought map (for XP rewards) */
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Combat")
		int32 MapLevel;

	/** Party used in recently fought map */
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Combat")
		TArray<class UCharacterDataAsset*> RecentParty;

	/** Key is ID, value is alive (true) or dead (false) */
	UPROPERTY(Transient, BlueprintReadWrite)
		TMap<int32, bool> RecentPartyIDs;

	UFUNCTION(BlueprintPure)
		static UGTGameInstance* GetGTGameInstance() { return Instance; }

	virtual void Init() override;

	virtual void Shutdown() override;

	FLinearColor GetReachColor(uint8 index);

	UFUNCTION(BlueprintCallable)
		static void GiveXP(class UCharacterDataAsset* character);
};
