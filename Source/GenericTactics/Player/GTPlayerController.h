// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "../Utility/ActionSourceInterface.h"
#include "../Utility/TargetableInterface.h"
#include "../Movement/NavGrid.h"
#include "GTPlayerController.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogGTPlayerController, Log, All);

UENUM(BlueprintType)
enum class EPCState : uint8
{
	Idle, //waiting on command
	Active, //moving or acting
	TargetMove, //planning move
	TargetAction, //planning action
};

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

	UPROPERTY(BlueprintReadOnly)
		EPCState PCState = EPCState::Idle;

	class UAction* SelectedAction;

	bool bHavePath = false;
	struct FNavPath NavPath;
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
		uint8 FinalRemainingActions;

	/** if cursor is over UI and should be blocked from actor hovering */
	UPROPERTY(BlueprintReadWrite)
		bool bUIBlockHover = false;

		UFUNCTION(BlueprintPure)
		EPCState GetState() { return PCState; }

	virtual void PlayerTick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, Category = "Player")
		void ControlCharacter(class AGTCharacter* chara);

	void MoveCompleted();

	UFUNCTION(BlueprintPure, Category = "Debug")
		static FString GetHoverText();

	UFUNCTION(BlueprintCallable, Category = "Player")
		void BeginTargetMove();

	UFUNCTION(BlueprintCallable, Category = "Player")
		void BeginTargetAction(class UAction* action);

	UFUNCTION(BlueprintCallable, Category = "Player")
		void CancelTarget();
};
