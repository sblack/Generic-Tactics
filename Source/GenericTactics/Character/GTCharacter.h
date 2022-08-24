// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
//#include "PaperZDCharacter.h"
#include "../Combat/CombatManager.h"
#include "../Movement/NavGrid.h"
#include "../Utility/GTBFL.h"
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

UENUM()
enum class ESpriteNames : uint8
{
	Idle,
	Attack,
	Point,
	Raise,
	Struck,
	Down,
	Num UMETA(Hidden)
};

/**
 * 
 */
UCLASS(ABSTRACT)
class GENERICTACTICS_API AGTCharacter : public APawn, public ITargetableInterface
{
	GENERATED_BODY()

//COMPONENTS AND PAWN
private:
	/** The CapsuleComponent being used for movement collision (by CharacterMovement). Always treated as being vertically aligned in simple collision check functions. */
	UPROPERTY(Category = Character, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		UCapsuleComponent* CapsuleComponent;

	/** reference to the body sprite component in child class */
	UPROPERTY(Category = Character, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		class UPaperSpriteComponent* BodySprite;

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
	class UPaperSpriteComponent* HandSprite;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	/** Returns CapsuleComponent subobject **/
	FORCEINLINE class UCapsuleComponent* GetCapsuleComponent() const { return CapsuleComponent; }

	UFUNCTION(BlueprintPure, Category = "Character")
		FORCEINLINE class AGTAIController* GetTacticsAI();

	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	//	class UCharacterStatsComponent* Stats;

	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	//	class UCharacterEquipmentComponent* Equipment;

	// Sets default values for this character's properties
	AGTCharacter(const FObjectInitializer& ObjectInitializer);

	virtual void PostInitializeComponents() override;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION(BlueprintCallable)
	void SetSprite(class UPaperSprite* sprite);



//COSMETIC AND DATA
private:
	/** scale of sprite when first spawned; used for flipping left-right */
	FVector OriginalScale;

	void InitMaterials();

	/** the component attachments seem to bug out if attaching to the socket they already occupy; so detach. SHOULD BE IMMEDIATELY FOLLOWED BY UpdateFacing(). */
	UFUNCTION(BlueprintCallable)
		void ResetAttachments();

	UFUNCTION(BlueprintCallable)
		void UpdateFacing();

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

public:
	UPROPERTY(BlueprintReadOnly, EditAnywhere, meta = (ExposeOnSpawn))
		class UCharacterDataAsset* CharacterData;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
		FText CharacterName;

	virtual FText GetTargetName() const override { return CharacterName; }



//MOVEMENT
private:
	float MoveTimePassed = 0;
	TArray<FMovementStep> MoveSteps;

	void FinishedMoving();

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





private:


protected:

	void BeginTurn_Implementation();

	void EndTurn_Implementation();

public:

	//separate from other sprites to make a walk cycle
	UPROPERTY(EditDefaultsOnly)
	TArray<class UPaperSprite*> SpritesWalk;

	UPROPERTY(EditDefaultsOnly, meta = (ArraySizeEnum = "ESpriteNames"))
	class UPaperSprite* Sprites[ESpriteNames::Num];


	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Instanced)
		class UActionAttack* DefaultMeleeAttack;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Instanced)
		class UActionAttack* DefaultRangedAttack;

	

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
		float CurrentAP;

	/** 0 = player */
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		uint8 Team = 1;

	UFUNCTION(BlueprintPure)
		bool IsPartyCharacter() const { return Team == 0; }

	//UPROPERTY(BlueprintReadWrite)
	//	EAnimState AnimState = EAnimState::Idle;

	UPROPERTY(BlueprintReadWrite)
		bool bDead = false;

	struct FActionData ActionInProgress;

	//UFUNCTION(BlueprintPure, Category = "Combat")
	//	EActionAnim GetActionAnim();

	UPROPERTY(BlueprintReadOnly)
		float Initiative = -1;

	UPROPERTY(BlueprintReadOnly)
		bool bIsMyTurn;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Combat")
		void BeginTurn();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Combat")
		void EndTurn();

	virtual int32 GetCurrentHealth() const override;

	virtual int32 GetMaxHealth() const override;

	UFUNCTION(BlueprintCallable, Category = "Combat")
		void ResolveAction();

	UFUNCTION(BlueprintCallable, Category = "Combat")
		void CompleteAction();

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Combat")
		void PlayActionAnim(EActionAnim anim);

	/*UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Combat")
		void PlayHitAnim(bool bMajor);*/

	float RollInitiative();

	//bool CanReact();

	//float GetAttackBonus();

	virtual int32 ApplyDamage(int32 amount, EDamageType damageType, EVitals vital) override;

	virtual void OnHit(bool bMajor) override;

	/*UFUNCTION(BlueprintImplementableEvent, Category = "Combat")
		void SetWeaponSprite(class UPaperSprite* weaponSprite);*/

	virtual void OnHoverStart() override;

	virtual void OnHoverEnd() override;

	bool IsSameTeam(ITargetable target);

	UFUNCTION(BlueprintCallable, Category = "Combat")
		void QueueAction(FNavPath path, FActionData actionData);

	UFUNCTION(BlueprintCallable, Category = "Combat")
		TArray<class UActionAttack*> GetAllAttacks();

	//virtual void PostLoad() override;
};
