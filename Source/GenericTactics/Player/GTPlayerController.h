// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "../Utility/TargetableInterface.h"
#include "GTPlayerController.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogGTPlayerController, Log, All);

/**
 * 
 */
UCLASS()
class GENERICTACTICS_API AGTPlayerController : public APlayerController
{
	GENERATED_BODY()
protected:
	UPROPERTY(BlueprintReadOnly)
		FVector HoverLocation;

	UPROPERTY(BlueprintReadOnly)
		TScriptInterface<ITargetableInterface> HoverTarget;

	UPROPERTY(BlueprintReadOnly)
		FVector SelectedLocation;

	UPROPERTY(BlueprintReadOnly)
		TScriptInterface<ITargetableInterface> SelectedTarget;

	UPROPERTY(BlueprintReadOnly)
		bool bSelecting;

	UPROPERTY(BlueprintReadOnly)
		class AGTCharacter* ActiveCharacter;

	bool bHavePath = false;
	TArray<FVector> NavPath;
	float NavPathCost;
	bool bMoving = false;

	AGTPlayerController();

	virtual void SetupInputComponent() override;

	virtual void BeginPlay() override;

	UFUNCTION(BlueprintImplementableEvent, Category = "Internal")
		void UpdateHover();

	void MoveForward(float value);
	void MoveRight(float value);
	void RotateCamera(float value);
	void Zoom(float value);
	void OnLeftClickUp();
	/** Pre-Combat */
	void OnLeftClickUpPre();
	/** Combat */
	void OnLeftClickUpCombat();

	void MouseOverTerrain(FVector location);
	void MouseOverTarget(ITargetable target);

public:

	static AGTPlayerController* Instance;

	UFUNCTION(BlueprintPure, Category = "Player")
		static AGTPlayerController* GetTacticsController() { return Instance; }

	UPROPERTY(BlueprintReadOnly)
		float FinalAP;

	UPROPERTY(BlueprintReadOnly)
		bool bTargetMove = false;

	UPROPERTY(BlueprintReadOnly)
		bool bTargetAction = false;

	virtual void PlayerTick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, Category = "Player")
		void ControlCharacter(class AGTCharacter* chara);

	void MoveCompleted();

	UFUNCTION(BlueprintPure, Category = "Debug")
		static FString GetHoverText();

	UFUNCTION(BlueprintCallable, Category = "Player")
		void BeginTargetMove();

	UFUNCTION(BlueprintCallable, Category = "Player")
		void BeginTargetAction();

	UFUNCTION(BlueprintCallable, Category = "Player")
		void CancelTarget();

	UFUNCTION(BlueprintCallable, Category = "Player")
		void UpdateFinalAP(float apCost);
};
