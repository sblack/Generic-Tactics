// Fill out your copyright notice in the Description page of Project Settings.


#include "HumanBase.h"
#include "../Player/GTPlayerController.h"
#include "TimerManager.h"



void AHumanBase::BeginPlay()
{
	if (CharacterData)
	{
		UE_LOG(LogTemp, Log, TEXT("Building from Data"));

		CharacterData = UCharacterDataAsset::RandomCopyCharacter(CharacterData, 0);
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("Building %s from Scratch"), *GetName());
		CharacterData = UCharacterDataAsset::NewCharacter();

		if (!IsPartyCharacter())
		{
			CharacterData->BodyColorsHSL = CharacterData->BodyColorsHSL.ChangeTeamHue(0, CharacterData->BodyAsset->TeamColors);
			CharacterData->HatColorsHSL = CharacterData->HatColorsHSL.ChangeTeamHue(0, CharacterData->HatAsset->TeamColors[CharacterData->HatIndex % 10]);
		}
	}

	Super::BeginPlay();

	CharacterName = CharacterData->Name;

	UpdateWeaponAndShield();

	//everything before this WAS before Super::BeginPlay

	if (GetAnimInstance())
	{
		if (GetAnimInstance()->GetPlayer())
		{
			GetAnimInstance()->GetPlayer()->OnPlaybackSequenceChanged.AddDynamic(this, &AHumanBase::OnAnimSequenceUpdated);
			UE_LOG(LogTemp, Log, TEXT("got anim player for %s"), *GetName());
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("could not get anim player for %s"), *GetName());
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("could not get anim instance for %s"), *GetName());
	}
}

void AHumanBase::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	if (IsPendingKill()) return;

	HairSprite = Cast<UPaperSpriteComponent>(GetDefaultSubobjectByName(TEXT("Hair")));
	HatFrontSprite = Cast<UPaperSpriteComponent>(GetDefaultSubobjectByName(TEXT("HatF")));
	HatBackSprite = Cast<UPaperSpriteComponent>(GetDefaultSubobjectByName(TEXT("HatB")));
	ShieldSprite = Cast<UPaperSpriteComponent>(GetDefaultSubobjectByName(TEXT("Shield")));
	WeaponSprite = Cast<UPaperSpriteComponent>(GetDefaultSubobjectByName(TEXT("Weapon")));
	HandSprite = Cast<UPaperFlipbookComponent>(GetDefaultSubobjectByName(TEXT("Hand")));
}

void AHumanBase::InitMaterials()
{
	BodyDMI = GetSprite()->CreateDynamicMaterialInstance(0);
	BodyDMI->SetTextureParameterValue(TEXT("Texture"), CharacterData->BodyAsset->Image);
	FLinearColor skinColor = UGTBFL::HSLtoRGB(CharacterData->SkinColorHSL);
	BodyDMI->SetVectorParameterValue(TEXT("ColorSkin"), skinColor);
	UColorQuartetBFL::SetColorQuartetParameter(BodyDMI, UColorQuartetBFL::HSLtoRGBQuartet(CharacterData->BodyColorsHSL));

	HandDMI = HandSprite->CreateDynamicMaterialInstance(0);
	if (CharacterData->BodyAsset->HandColor == 4)
		HandDMI->SetVectorParameterValue(TEXT("Color"), skinColor);
	else
		HandDMI->SetVectorParameterValue(TEXT("Color"), UGTBFL::HSLtoRGB(UColorQuartetBFL::GetQuartetMember(CharacterData->BodyColorsHSL, CharacterData->BodyAsset->HandColor)));

	HairDMI = HairSprite->CreateDynamicMaterialInstance(0);
	if (CharacterData->HairIndex == 255) //bald
		HairSprite->SetVisibility(false);
	else
	{
		//HairDMI->SetTextureParameterValue(TEXT("Texture"), );
		HairDMI->SetScalarParameterValue(TEXT("Column"), CharacterData->HairIndex / 5);
		HairDMI->SetScalarParameterValue(TEXT("Row"), CharacterData->HairIndex % 5);
		HairDMI->SetVectorParameterValue(TEXT("Color"), UGTBFL::HSLtoRGB(CharacterData->HairColorHSL));
	}

	HatFrontDMI = HatFrontSprite->CreateDynamicMaterialInstance(0);
	HatBackDMI = HatBackSprite->CreateDynamicMaterialInstance(0);
	if (CharacterData->HatIndex == 255) //no hat
	{
		HatFrontSprite->SetVisibility(false);
		HatBackSprite->SetVisibility(false);
	}
	else
	{
		HatFrontDMI->SetTextureParameterValue(TEXT("Texture"), CharacterData->HatAsset->Image);
		HatBackDMI->SetTextureParameterValue(TEXT("Texture"), CharacterData->HatAsset->ImageB);
		HatFrontDMI->SetScalarParameterValue(TEXT("Column"), CharacterData->HatIndex / 5);
		HatFrontDMI->SetScalarParameterValue(TEXT("Row"), CharacterData->HatIndex % 5);
		HatBackDMI->SetScalarParameterValue(TEXT("Column"), CharacterData->HatIndex / 5);
		HatBackDMI->SetScalarParameterValue(TEXT("Row"), CharacterData->HatIndex % 5);
		FColorQuartet hatColorsRGB = UColorQuartetBFL::HSLtoRGBQuartet(CharacterData->HatColorsHSL);
		UColorQuartetBFL::SetColorQuartetParameter(HatFrontDMI, hatColorsRGB);
		UColorQuartetBFL::SetColorQuartetParameter(HatBackDMI, hatColorsRGB);
	}
}

void AHumanBase::ResetAttachments()
{
	FAttachmentTransformRules atr = FAttachmentTransformRules(EAttachmentRule::KeepRelative, true);
	HairSprite->AttachToComponent(GetSprite(), atr);
	HatFrontSprite->AttachToComponent(GetSprite(), atr);
	HatBackSprite->AttachToComponent(GetSprite(), atr);

	WeaponSprite->AttachToComponent(GetSprite(), atr);
	ShieldSprite->AttachToComponent(GetSprite(), atr);

	UpdateFacing();
}

void AHumanBase::OnAnimSequenceUpdated(const class UPaperZDAnimSequence* From, const class UPaperZDAnimSequence* To, float CurrentProgress)
{
	HandSprite->SetFlipbook(To->GetAnimationData<UPaperFlipbook*>());
	HandSprite->SetPlaybackPosition(CurrentProgress, false);
	ResetAttachments();
}

FTransform AHumanBase::GetTipTransform()
{
	if (WeaponSprite->DoesSocketExist(TEXT("Tip")))
		return WeaponSprite->GetSocketTransform(TEXT("Tip"));
	else
		return GetSprite()->GetSocketTransform(WeaponSocketName);
}

void AHumanBase::SetSpriteColor(FLinearColor color)
{
	GetSprite()->SetSpriteColor(color);
	HairSprite->SetSpriteColor(color);
	HatBackSprite->SetSpriteColor(color);
	HatFrontSprite->SetSpriteColor(color);
	HandSprite->SetSpriteColor(color);
	WeaponSprite->SetSpriteColor(color);
	ShieldSprite->SetSpriteColor(color);
}

void AHumanBase::UpdateFacing()
{
	float diff = FMath::Fmod((GetActorRotation().Yaw - UGameplayStatics::GetPlayerCameraManager(this, 0)->GetCameraRotation().Yaw + 360.0f), 360.0f);
	float bodyFB, headFB, headFlip;
	if (diff < 90) {
		bodyFB = 2; headFB = 1; headFlip = 1;
		HeadSocketName = FName(TEXT("Head_2")); ShieldSocketName = FName(TEXT("Shield_2")); WeaponSocketName = FName(TEXT("Weapon_2"));
	}
	else if (diff < 180) {
		bodyFB = 1; headFB = 0; headFlip = -1;
		HeadSocketName = FName(TEXT("Head_1")); ShieldSocketName = FName(TEXT("Shield_1")); WeaponSocketName = FName(TEXT("Weapon_1"));
	}
	else if (diff < 270) {
		bodyFB = 0; headFB = 0; headFlip = 1;
		HeadSocketName = FName(TEXT("Head_0")); ShieldSocketName = FName(TEXT("Shield_0")); WeaponSocketName = FName(TEXT("Weapon_0"));
	}
	else {
		bodyFB = 3; headFB = 1; headFlip = -1;
		HeadSocketName = FName(TEXT("Head_3")); ShieldSocketName = FName(TEXT("Shield_3")); WeaponSocketName = FName(TEXT("Weapon_3"));
	}

	BodyDMI->SetScalarParameterValue(TEXT("FrontBack"), bodyFB);
	HandDMI->SetScalarParameterValue(TEXT("FrontBack"), bodyFB);
	HairDMI->SetScalarParameterValue(TEXT("FrontBack"), headFB);
	HatFrontDMI->SetScalarParameterValue(TEXT("FrontBack"), headFB);
	HatBackDMI->SetScalarParameterValue(TEXT("FrontBack"), headFB);

	FAttachmentTransformRules atr = FAttachmentTransformRules(EAttachmentRule::KeepRelative, true);
	HairSprite->AttachToComponent(GetSprite(), atr, HeadSocketName);
	HatFrontSprite->AttachToComponent(GetSprite(), atr, HeadSocketName);
	HatBackSprite->AttachToComponent(GetSprite(), atr, HeadSocketName);

	WeaponSprite->AttachToComponent(GetSprite(), atr, WeaponSocketName);
	ShieldSprite->AttachToComponent(GetSprite(), atr, ShieldSocketName);

	HairSprite->SetRelativeScale3D(FVector(headFlip, 1, 1));
	HatFrontSprite->SetRelativeScale3D(FVector(headFlip, 1, 1));
	HatBackSprite->SetRelativeScale3D(FVector(headFlip, 1, 1));

	UGTBFL::FaceCamera(this, GetSprite());
}

void AHumanBase::UpdateWeaponAndShield()
{
	if (CharacterData->Weapon.BaseItem)
	{
		WeaponSprite->SetSprite(CharacterData->Weapon.BaseItem->Sprite);
		WeaponSprite->SetVisibility(true);
	}
	else
	{
		WeaponSprite->SetVisibility(false);
	}

	if (CharacterData->Shield.BaseItem)
	{
		ShieldSprite->SetSprite(CharacterData->Shield.BaseItem->Sprite);
		ShieldSprite->SetVisibility(true);
	}
	else
	{
		ShieldSprite->SetVisibility(false);
	}
}

void AHumanBase::AdvanceAI()
{
	if (RemainingActions == 0 && RemainingMove < 1)
	{
		FTimerHandle TimerHandle;
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &AGTCharacter::EndTurn, 1, false);
		return;
	}

	//TODO: allow for AI control on Party Characters?
	if (IsPartyCharacter())
	{
		AGTPlayerController::Instance->AwaitInput();
		return;
	}

	if (CharacterData) 
	{
		bool bEndTurn = true; //if AI can't perform any objectives, end turn
		for (int i = 0; i < CharacterData->AIObjectives.Num(); i++)
		{
			if (CharacterData->AIObjectives[i]->Attempt(this))
			{
				bEndTurn = false;
				UE_LOG(LogTemp, Log, TEXT("%s successful"), *CharacterData->AIObjectives[i]->DebugString);
				break;
			}
			else
			{
				UE_LOG(LogTemp, Log, TEXT("%s failed"), *CharacterData->AIObjectives[i]->DebugString);
			}
		}

		if (bEndTurn)
		{
			UE_LOG(LogTemp, Log, TEXT("all objectives impossible, ending turn"));

			FTimerHandle TimerHandle;
			GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &AGTCharacter::EndTurn, 1, false);
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("%s has no character data and no AI. Ending turn."), *GetDisplayName().ToString());
		FTimerHandle TimerHandle;
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &AGTCharacter::EndTurn, 1, false);
	}
}

void AHumanBase::SetStats()
{
	Stats->FillFromData(CharacterData);
}

TArray<class UAction*> AHumanBase::GetAllAttacks()
{
	TArray<UAction*> attacks;

	if (CharacterData->Weapon.BaseItem)
	{
		attacks.Add(UActionWeapon::CreateDefaultAttack(this));
	}

	if (DefaultMeleeAttack)
		attacks.Add(DefaultMeleeAttack);
	if (DefaultRangedAttack)
		attacks.Add(DefaultRangedAttack);

	//TODO: get weapon attacks
	//TODO: get skill attacks


	return attacks;
}
