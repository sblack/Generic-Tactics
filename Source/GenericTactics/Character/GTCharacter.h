// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PaperZDCharacter.h"
#include "../Combat/CombatManager.h"
#include "../Movement/NavGrid.h"
#include "../Utility/GTBFL.h"
#include "../Utility/ActionSourceInterface.h"
#include "../Utility/TargetableInterface.h"
#include "../Utility/DirectionalSpriteInterface.h"
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
class GENERICTACTICS_API AGTCharacter : public APaperZDCharacter, public ITargetableInterface, public IActionSourceInterface, public IDirectionalSpriteInterface
{
	GENERATED_BODY()
//INHERITED
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	UFUNCTION(BlueprintPure, Category = "Character")
		FORCEINLINE class AGTAIController* GetTacticsAI();

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

	//virtual void PostLoad() override;

	void EndPlay(const EEndPlayReason::Type EndPlayReason) override;





	//SPRITES AND COSMETIC
private:
	/** reference to the hair sprite component in child class */
	class UPaperSpriteComponent* HairSprite;

	/** reference to the hat front sprite component in child class */
	class UPaperSpriteComponent* HatFrontSprite;

	/** reference to the hat back sprite component in child class */
	class UPaperSpriteComponent* HatBackSprite;

	/** reference to the shield sprite component in child class */
	class UPaperSpriteComponent* ShieldSprite;

	/** reference to the weapon sprite component in child class */
	class UPaperSpriteComponent* WeaponSprite;

	/** reference to the hand sprite component in child class */
	class UPaperFlipbookComponent* HandSprite;

	FName HeadSocketName;
	FName WeaponSocketName;
	FName ShieldSocketName;

	/** scale of sprite when first spawned; used for flipping left-right */
	FVector OriginalScale;

	UPROPERTY(EditDefaultsOnly)
		class UPaperFlipbook* FBIdle;

	UPROPERTY(EditDefaultsOnly)
		class UPaperFlipbook* FBWalk;

	void InitMaterials();

	/** the component attachments seem to bug out if attaching to the socket they already occupy; so detach, then reattach */
	UFUNCTION(BlueprintCallable)
		void ResetAttachments();

	UFUNCTION()
		void OnAnimSequenceUpdated(const class UPaperZDAnimSequence* From, const class UPaperZDAnimSequence* To, float CurrentProgress);

protected:
	UPROPERTY(BlueprintReadOnly)
		class UMaterialInstanceDynamic* BodyDMI;

	UPROPERTY(BlueprintReadOnly)
		class UMaterialInstanceDynamic* HairDMI;

	UPROPERTY(BlueprintReadOnly)
		class UMaterialInstanceDynamic* HatFrontDMI;

	UPROPERTY(BlueprintReadOnly)
		class UMaterialInstanceDynamic* HatBackDMI;

	UPROPERTY(BlueprintReadOnly)
		class UMaterialInstanceDynamic* HandDMI;

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Combat")
		void PlayHitAnim(bool bMajor);

public:
	UPROPERTY(BlueprintReadWrite)
		EStatusAnim StatusAnim = EStatusAnim::Idle;

		/**SetSpriteColor for all sprite and flipbook components*/
	UFUNCTION(BlueprintCallable)
		void SetSpriteColor(FLinearColor color);

	UFUNCTION(BlueprintCallable)
		virtual void UpdateFacing() override;




//MOVEMENT
private:
	float MoveTimePassed = 0;
	TArray<FMovementStep> MoveSteps;

	void FinishedMoving();
	/** called on entering each step/tile of movement */
	void OnMoveStep();

	const FNodeData* FindMoveData(FVector vec) const;

public:
	/** is moving */
	UPROPERTY(BlueprintReadOnly)
		bool bIsMoving = false;

	TArray< TArray<FNodeData>> MoveGrid;
	int MoveDataID;

	UFUNCTION(BlueprintCallable, Category = "Combat")
		bool GetPathTo(FVector destination, FNavPath& path);

	void StartMoving(FNavPath path);

	UFUNCTION(BlueprintPure, Category = "Tactics")
		TArray<FNodeData> GetReachableArea();

	/** @param target Target location
	 @param range Desired distance from target */
	UFUNCTION(BlueprintPure, Category = "Tactics")
		FNodeData NearestReachableLocationToTarget(FVector target, float range);

	UFUNCTION(BlueprintCallable)
		void TurnToFace(FVector target);



	//ACTIONS
private:

	void StartAction();

public:
	UPROPERTY(BlueprintReadOnly)
		struct FActionData ActionInProgress;

	/** If action uses a projectile, action should complete based on projectile, not animation */
	UPROPERTY(BlueprintReadOnly)
		bool bWaitingOnProjectile = false;

	UFUNCTION(BlueprintPure, Category = "Combat")
		EActionAnim GetActionAnim();

	/** Performs action immediately (assumes actor is already within range) */
	UFUNCTION(BlueprintCallable, Category = "Combat")
		void PerformAction(FActionData actionData);

	UFUNCTION(BlueprintCallable, Category = "Combat")
		void ResolveAction();

	UFUNCTION(BlueprintCallable, Category = "Combat")
		void CompleteAction();

	/*UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Combat")
		void PlayActionAnim(EActionAnim anim);*/

	/** Move into range to perform action, or perform if already in range */
	UFUNCTION(BlueprintCallable, Category = "Combat")
		void QueueAction(FNavPath path, FActionData actionData);

	/** Fires projectile IF ActionInProgress has one */
	UFUNCTION(BlueprintCallable, Category = "Combat")
		void FireProjectile();

	/** TODO: handle better */
	UFUNCTION(BlueprintCallable, Category = "Combat")
		void Meditate();



//STATS
public:
	UPROPERTY(Transient, BlueprintReadOnly)
		class UStatsBlock* Stats;

	virtual int32 GetCurrentHealth() const override;

	virtual int32 GetMaxHealth() const override;

	virtual int32 GetDefense(EAttackType attack) const;

	UFUNCTION(BlueprintCallable, Category = "Combat")
	int32 GetAccuracy(EAttackType attack) const;

	UFUNCTION(BlueprintCallable, Category = "Combat")
	bool GetIsDead() const { return GetCurrentHealth() == 0; }




protected:

	UPROPERTY(BlueprintReadOnly)
		float Initiative = -1;

	void BeginTurn_Implementation();

	void EndTurn_Implementation();

public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Instanced)
		class UActionAttack* DefaultMeleeAttack;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Instanced)
		class UActionAttack* DefaultRangedAttack;

	

	UPROPERTY(BlueprintReadOnly, EditAnywhere, meta = (ExposeOnSpawn))
		class UCharacterDataAsset* CharacterData;

	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	//	class UCharacterEquipmentComponent* Equipment;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
		FText CharacterName;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
		float CurrentAP;

	/** 0 = player */
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		uint8 Team = 1;

	UFUNCTION(BlueprintPure)
		bool IsPartyCharacter() const { return Team == 0; }

	//UFUNCTION(BlueprintPure, Category = "Combat")
	//	EActionAnim GetActionAnim();

	float GetInitiative() override { return Initiative; }

	UPROPERTY(BlueprintReadOnly)
		bool bIsMyTurn;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Combat")
		void BeginTurn();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Combat")
		void EndTurn();

	virtual FText GetDisplayName() const override { return CharacterName; }

	float RollInitiative();

	//bool CanReact();

	virtual int32 ApplyDamage(int32 amount, EDamageType damageType, EVitals vital) override;

	virtual void OnHit(bool bMajor) override;

	/*UFUNCTION(BlueprintImplementableEvent, Category = "Combat")
		void SetWeaponSprite(class UPaperSprite* weaponSprite);*/

	virtual void OnHoverStart() override;

	virtual void OnHoverEnd() override;

	bool IsSameTeam(ITargetable target);

	UFUNCTION(BlueprintCallable, Category = "Combat")
		TArray<class UActionAttack*> GetAllAttacks();
};
