// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GTCharacter.h"
#include "HumanBase.generated.h"

/**
 * 
 */
UCLASS(abstract)
class GENERICTACTICS_API AHumanBase : public AGTCharacter
{
	GENERATED_BODY()
	//INHERITED
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:

	virtual void PostInitializeComponents() override;




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

	virtual void InitMaterials() override;

	/** the component attachments seem to bug out if attaching to the socket they already occupy; so detach, then reattach */
	virtual void ResetAttachments() override;

	UFUNCTION()
		void OnAnimSequenceUpdated(const class UPaperZDAnimSequence* From, const class UPaperZDAnimSequence* To, float CurrentProgress);

protected:
	UPROPERTY(BlueprintReadOnly)
		class UMaterialInstanceDynamic* HairDMI;

	UPROPERTY(BlueprintReadOnly)
		class UMaterialInstanceDynamic* HatFrontDMI;

	UPROPERTY(BlueprintReadOnly)
		class UMaterialInstanceDynamic* HatBackDMI;

	UPROPERTY(BlueprintReadOnly)
		class UMaterialInstanceDynamic* HandDMI;

	virtual FTransform GetTipTransform() override;

public:

	/**SetSpriteColor for all sprite and flipbook components*/
	virtual void SetSpriteColor(FLinearColor color) override;

	virtual void UpdateFacing() override;

	UFUNCTION(BlueprintCallable)
		void UpdateWeaponAndShield();



protected:
	virtual void AdvanceAI() override;




protected:
	virtual void SetStats() override;


public:
	UPROPERTY(BlueprintReadOnly, EditAnywhere, meta = (ExposeOnSpawn))
		class UCharacterDataAsset* CharacterData;

	virtual TArray<class UActionAttack*> GetAllAttacks() override;
};
