// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PaperZDCharacter.h"
#include "../Combat/CombatManager.h"
#include "../Utility/TargetableInterface.h"
#include "CharacterDataAsset.h"
#include "GTCharacter.generated.h"

struct FMovementStep
{
	float TimeToArrival;
	FVector StartLocation;
	FVector Velocity;
	bool bJump;

	FMovementStep(FVector start, FVector end);
	FVector CalcPosition(float time);
};

/**
 * 
 */
UCLASS(ABSTRACT)
class GENERICTACTICS_API AGTCharacter : public APaperZDCharacter, public ITargetableInterface
{
	GENERATED_BODY()
private:
	/** reference to the hair sprite component in child class */
	class UPaperSpriteComponent* HairSprite;

	/** reference to the hat front sprite component in child class */
	class UPaperSpriteComponent* HatFrontSprite;

	/** reference to the hat back sprite component in child class */
	class UPaperSpriteComponent* HatBackSprite;

	/** scale of sprite when first spawned; used for flipping left-right */
	FVector OriginalScale;

	bool bIsMoving = false;
	float MoveTimePassed = 0;
	TArray<FMovementStep> MoveSteps;

	void InitMaterials();

	UFUNCTION(BlueprintCallable)
	void FrontBackFlip();

protected:

	UPROPERTY(BlueprintReadOnly)
		class UMaterialInstanceDynamic* BodyDMI;

	UPROPERTY(BlueprintReadOnly)
		class UMaterialInstanceDynamic* HairDMI;

	UPROPERTY(BlueprintReadOnly)
		class UMaterialInstanceDynamic* HatFrontDMI;

	UPROPERTY(BlueprintReadOnly)
		class UMaterialInstanceDynamic* HatBackDMI;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void OnBeginTurn_Implementation();

	void OnEndTurn_Implementation() {}

public:
	/** flip sprite left-right */
	UPROPERTY(BlueprintReadOnly)
		bool bMirrored;

	/** front or back version of sprite */
	UPROPERTY(BlueprintReadOnly)
		bool bFrontView;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, meta = (ExposeOnSpawn))
		class UCharacterDataAsset* CharacterData;

	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	//	class UCharacterStatsComponent* Stats;

	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	//	class UCharacterEquipmentComponent* Equipment;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
		FText CharacterName;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
		float CurrentAP;

	/** 0 = player */
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		uint8 Team = 1;

	//UPROPERTY(BlueprintReadWrite)
	//	EAnimState AnimState = EAnimState::Idle;

	UPROPERTY(BlueprintReadWrite)
		bool bDead = false;

	struct FActionData ActionInProgress;

	//UFUNCTION(BlueprintPure, Category = "Combat")
	//	EActionAnim GetActionAnim();

	// Sets default values for this character's properties
	AGTCharacter(const FObjectInitializer& ObjectInitializer);

	virtual void PostInitializeComponents() override;

//#if WITH_EDITOR
//	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
//#endif

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION(BlueprintNativeEvent, Category = "Combat")
		void OnBeginTurn();

	UFUNCTION(BlueprintNativeEvent, Category = "Combat")
		void OnEndTurn();

	UFUNCTION(BlueprintPure, Category = "Character")
		FORCEINLINE class AGTAIController* GetTacticsAI();

	virtual FText GetTargetName() const override { return CharacterName; }

	virtual int32 GetCurrentHealth() const override;

	virtual int32 GetMaxHealth() const override;

	/*UFUNCTION(BlueprintCallable, Category = "Combat")
		void ResolveAction();

	UFUNCTION(BlueprintCallable, Category = "Combat")
		void CompleteAction();

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Combat")
		void PlayActionAnim(EActionAnim anim);

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Combat")
		void PlayHitAnim(bool bMajor);*/

	float RollInitiative();

	//bool CanReact();

	//float GetAttackBonus();

	virtual int32 ApplyDamage(int32 amount, EDamageType damageType, EVitals vital) override;

	virtual void OnHit(bool bMajor) override;

	/*UFUNCTION(BlueprintPure, Category = "Combat")
		class UActionAttack* GetMainAttack();

	UFUNCTION(BlueprintPure, Category = "Combat")
		class UActionAttack* GetOffAttack();

	UFUNCTION(BlueprintImplementableEvent, Category = "Combat")
		void SetWeaponSprite(class UPaperSprite* weaponSprite);*/

	virtual void OnHoverStart() override;

	virtual void OnHoverEnd() override;

	void StartMoving(TArray<FVector> path);


	//virtual void PostLoad() override;
};
