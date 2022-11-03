// Fill out your copyright notice in the Description page of Project Settings.


#include "GTBFL.h"
#include "UObject/UObjectGlobals.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Camera/PlayerCameraManager.h"

UEnum* UGTBFL::VitalsPtr;
UEnum* UGTBFL::DamagePtr;

void UGTBFL::FaceCamera(class AActor* actor, class USceneComponent* component)
{
	if (!actor)
	{
		UE_LOG(LogTemp, Error, TEXT("FaceCamera needs an actor"));
		return;
	}

	float roll = UKismetMathLibrary::FindLookAtRotation(UGameplayStatics::GetPlayerCameraManager(actor, 0)->GetCameraLocation(), actor->GetActorLocation()).Pitch; //pitch becomes roll
	float yaw = UGameplayStatics::GetPlayerCameraManager(actor, 0)->GetCameraRotation().Yaw;

	if (component)
		component->SetWorldRotation(UKismetMathLibrary::ComposeRotators(FRotator(0, yaw, roll), FRotator(0, 90, 0)));
	else
		actor->GetRootComponent()->SetWorldRotation(UKismetMathLibrary::ComposeRotators(FRotator(0, yaw, roll), FRotator(0, 90, 0)));
}

FLinearColor UGTBFL::HSLtoRGB(float H, float S, float L, float A)
{
	float c = S * (1 - FMath::Abs(2 * L - 1));
	float max = c / 2 + L;
	float med = c * (.5 - FMath::Abs(FMath::Fmod(H / 60, 2) - 1)) + L;
	float min = L - c / 2;

	if (H < 60)
		return FLinearColor(max, med, min, A);
	else if (H < 120)
		return FLinearColor(med, max, min, A);
	else if (H < 180)
		return FLinearColor(min, max, med, A);
	else if (H < 240)
		return FLinearColor(min, med, max, A);
	else if (H < 300)
		return FLinearColor(med, min, max, A);
	else
		return FLinearColor(max, min, med, A);
}

FLinearColor UGTBFL::HSLtoRGB(FLinearColor InHSL)
{
	return HSLtoRGB(InHSL.R, InHSL.G, InHSL.B, InHSL.A);
}

void UGTBFL::RGBToHSL(FLinearColor InColor, float& H, float& S, float& L, float& A)
{
	float max = FMath::Max(FMath::Max(InColor.R, InColor.G), InColor.B);
	float min = FMath::Min(FMath::Min(InColor.R, InColor.G), InColor.B);
	float c = max - min;

	if (c < .001)
	{
		H = 0;
		L = (max + min) / 2;
		S = 0;
		A = InColor.A;
		return;
	}

	if (max == InColor.R) H = FMath::Fmod((InColor.G - InColor.B) / c + 6, 6) * 60;
	else if (max == InColor.G) H = ((InColor.B - InColor.R) / c + 2) * 60;
	else H = ((InColor.R - InColor.G) / c + 4) * 60;

	L = (max + min) / 2;
	S = c / (1 - FMath::Abs(2 * L - 1));
	A = InColor.A;
}

FLinearColor UGTBFL::RGBToHSL(FLinearColor InRGB)
{
	float H, S, L;
	float max = FMath::Max(FMath::Max(InRGB.R, InRGB.G), InRGB.B);
	float min = FMath::Min(FMath::Min(InRGB.R, InRGB.G), InRGB.B);
	float c = max - min;

	if (c < .001)
	{
		return FLinearColor(0, (max + min) / 2, 0, InRGB.A);
	}

	if (max == InRGB.R) H = FMath::Fmod((InRGB.G - InRGB.B) / c + 6, 6) * 60;
	else if (max == InRGB.G) H = ((InRGB.B - InRGB.R) / c + 2) * 60;
	else H = ((InRGB.R - InRGB.G) / c + 4) * 60;

	L = (max + min) / 2;
	S = c / (1 - FMath::Abs(2 * L - 1));

	return FLinearColor(H, S, L, InRGB.A);
}

UEnum* UGTBFL::Vitals()
{
	if (VitalsPtr == nullptr)
	{
		//VitalsPtr = FindObject<UEnum>(ANY_PACKAGE, TEXT("EVitals"), true);
	}
	return VitalsPtr;
};

UEnum* UGTBFL::Damage()
{
	if (DamagePtr == nullptr)
	{
		//DamagePtr = FindObject<UEnum>(ANY_PACKAGE, TEXT("EDamage"), true);
	}
	return DamagePtr;
};

FText UGTBFL::VitalToText(EVitals vital)
{
	switch (vital)
	{
	case EVitals::Health: return NSLOCTEXT("Vitals", "Health", "Health");
	case EVitals::Mana: return NSLOCTEXT("Vitals", "Mana", "Mana");
	case EVitals::Stamina: return NSLOCTEXT("Vitals", "Stamina", "Stamina");
	default:
		return FText::FromString("ERROR");
	}
}

FText UGTBFL::DamageToText(EDamageType damage)
{
	switch (damage)
	{
	case EDamageType::Piercing:	return NSLOCTEXT("Damage", "Piercing", "Piercing");
	case EDamageType::Crushing:	return NSLOCTEXT("Damage", "Crushing", "Crushing");
	case EDamageType::Slashing:	return NSLOCTEXT("Damage", "Slashing", "Slashing");
	case EDamageType::Fire:	return NSLOCTEXT("Damage", "Fire", "Fire");
	case EDamageType::Acid:	return NSLOCTEXT("Damage", "Acid", "Acid");
	case EDamageType::Cold:	return NSLOCTEXT("Damage", "Cold", "Cold");
	case EDamageType::Electric:	return NSLOCTEXT("Damage", "Electric", "Electric");
	case EDamageType::Typeless:	return NSLOCTEXT("Damage", "Typeless", "Typeless");
	default:
		return FText::FromString("ERROR");
	}
}

FText UGTBFL::AttackToText(EAttackType attack)
{
	switch (attack)
	{
	case EAttackType::Melee:	return NSLOCTEXT("Attack", "Melee", "Melee");
	case EAttackType::Range:	return NSLOCTEXT("Attack", "Ranged", "Ranged");
	case EAttackType::Magic:	return NSLOCTEXT("Attack", "Magic", "Magic");
	default:
		return FText::FromString("ERROR");
	}
}

FText UGTBFL::DefenseToText(EDefenseType defense)
{
	switch (defense)
	{
	case EDefenseType::Armor:	return NSLOCTEXT("Defense", "Armor", "Armor");
	case EDefenseType::Reflex:	return NSLOCTEXT("Defense", "Reflex", "Reflex");
	case EDefenseType::Fortitude:	return NSLOCTEXT("Defense", "Fortitude", "Fortitude");
	case EDefenseType::Will:	return NSLOCTEXT("Defense", "Will", "Will");
	default:
		return FText::FromString("ERROR");
	}
}

FText UGTBFL::CharStatToText(ECharStat stat)
{
	switch (stat)
	{
	case ECharStat::Str:	return NSLOCTEXT("CharStat", "Str", "STR");
	case ECharStat::Dex:	return NSLOCTEXT("CharStat", "Dex", "DEX");
	case ECharStat::Con:	return NSLOCTEXT("CharStat", "Con", "CON");
	case ECharStat::Int:	return NSLOCTEXT("CharStat", "Int", "INT");
	case ECharStat::Wis:	return NSLOCTEXT("CharStat", "Wis", "WIS");
	case ECharStat::None:	return NSLOCTEXT("CharStat", "None", "NONE");
	default:
		return FText::FromString("ERROR");
	}
}
