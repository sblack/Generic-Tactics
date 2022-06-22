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
	void LoadData();

public:
	static UGTGameInstance* Instance;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		TArray<UAppearanceAsset*> BodyTypes;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		TArray<UAppearanceSetAsset*> HatTypes;

	UFUNCTION(BlueprintPure)
		static UGTGameInstance* GetGTGameInstance() { return Instance; }

	virtual void Init() override;

	virtual void Shutdown() override;
};
