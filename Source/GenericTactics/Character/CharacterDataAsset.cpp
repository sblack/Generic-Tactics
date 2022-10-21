// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterDataAsset.h"
#include "../GTGameInstance.h"
#include "../Feats/Feat.h"
#include "../Utility/GTBFL.h"

UCharacterDataAsset* UCharacterDataAsset::FromSave(struct FCharacterData saveData)
{
	UCharacterDataAsset* result = NewObject<UCharacterDataAsset>();
	result->Name = saveData.Name;
	result->ID = saveData.ID;

	result->SkinColorHSL = saveData.SkinColorHSL;
	result->HairIndex = saveData.HairIndex;
	result->HairColorHSL = saveData.HairColorHSL;
	result->BodyAsset = saveData.BodyAsset;
	result->BodyColorsHSL = saveData.BodyColorsHSL;
	result->HatAsset = saveData.HatAsset;
	result->HatIndex = saveData.HatIndex;
	result->HatColorsHSL = saveData.HatColorsHSL;
	
	/*result->Weapon = saveData.Weapon;
	result->Shield = saveData.Shield;
	result->Armor = saveData.Armor;
	result->Accessories = saveData.Accessories;*/

	result->TotalXP = saveData.TotalXP;
	result->RemainingXP = saveData.RemainingXP;
	for (UFeat* feat : saveData.Feats)
	{
		result->Feats.Add(feat);
		if(feat->bChassisFeat) result->ChassisFeatCount += 1;
	}

	//start the character at level 1 XP (if below)
	if (result->TotalXP < UGTGameInstance::Instance->XPPerLevel)
	{
		result->RemainingXP += UGTGameInstance::Instance->XPPerLevel - result->TotalXP;
		result->TotalXP = UGTGameInstance::Instance->XPPerLevel;
	}

	return result;
}

UCharacterDataAsset* UCharacterDataAsset::NewCharacter()
{
	FCharacterData data;
	data.Randomize();

	return FromSave(data);
}

UCharacterDataAsset* UCharacterDataAsset::RandomCopyCharacter(UCharacterDataAsset* source, float teamHue)
{
	/*UCharacterDataAsset* result = DuplicateObject<UCharacterDataAsset>(source, source->GetOuter());*/
	UCharacterDataAsset* result = NewObject<UCharacterDataAsset>(source->GetOuter(), NAME_None, RF_NoFlags, source);

	if(result->HairIndex == 255)
		result->HairIndex = FMath::RandRange(0, 9);

	if(result->Name.IsEmptyOrWhitespace())
	{
		if (result->HairIndex < 5)
		{
			result->Name = FText::FromStringTable("/Game/Data/Character/NamesMale", FString::Printf(TEXT("Male_%03d"), FMath::RandRange(0, 255)));
		}
		else
		{
			result->Name = FText::FromStringTable("/Game/Data/Character/NamesFemale", FString::Printf(TEXT("Female_%03d"), FMath::RandRange(0, 255)));
		}
	}

	if(result->SkinColorHSL == FLinearColor::Transparent) // 0,0,0,0
	{
		float k = FMath::FRandRange(0, 3);
		if (k < 1) 
			result->SkinColorHSL = FLinearColor(20, .6 + k * (.375 - .6), .75 + k * (.6 - .75));
		else if (k < 2) 
			result->SkinColorHSL = FLinearColor(20 + (k - 1) * (28 - 20), .375 + (k - 1) * (.538 - .375), .6 + (k - 1) * (.325 - .6));
		else 
			result->SkinColorHSL = FLinearColor(28 + (k - 2) * (15 - 28), .538 + (k - 2) * (.5 - .538), .325 + (k - 2) * (.2 - .325));
	}

	//SkinColorHSL = FLinearColor(20, .375f, .6f);

	if (result->HairColorHSL == FLinearColor::Transparent) // 0,0,0,0
	{
		result->HairColorHSL = FLinearColor(FMath::FRandRange(0, 60), FMath::FRandRange(0, 1),
			((FMath::RandRange(0, 9) == 0) ? FMath::FRandRange(.8f, .9f) : FMath::FRandRange(.1f, result->SkinColorHSL.B)));
	}

	if(!result->BodyAsset)
		result->BodyAsset = UGTGameInstance::Instance->BodyTypes[FMath::RandRange(0, UGTGameInstance::Instance->BodyTypes.Num() - 1)];

	if (result->BodyColorsHSL.Color0 == FLinearColor::Transparent)
	{
		result->BodyColorsHSL.Color0 = UGTBFL::RGBToHSL(result->BodyAsset->DefaultColorsRGB.Color0);
		result->BodyColorsHSL.Color1 = UGTBFL::RGBToHSL(result->BodyAsset->DefaultColorsRGB.Color1);
		result->BodyColorsHSL.Color2 = UGTBFL::RGBToHSL(result->BodyAsset->DefaultColorsRGB.Color2);
		result->BodyColorsHSL.Color3 = UGTBFL::RGBToHSL(result->BodyAsset->DefaultColorsRGB.Color3);
		result->BodyColorsHSL = result->BodyColorsHSL.ChangeTeamHue(teamHue, result->BodyAsset->TeamColors);
	}

	if (!result->HatAsset)
		result->HatAsset = UGTGameInstance::Instance->HatTypes[FMath::RandRange(0, UGTGameInstance::Instance->HatTypes.Num() - 1)];

	if(result->HatIndex == 255)
		result->HatIndex = FMath::RandRange(0, 9);

	if(result->HatColorsHSL.Color0 == FLinearColor::Transparent)
	{
		result->HatColorsHSL.Color0 = UGTBFL::RGBToHSL(result->HatAsset->DefaultColorsRGB[result->HatIndex].Color0);
		result->HatColorsHSL.Color1 = UGTBFL::RGBToHSL(result->HatAsset->DefaultColorsRGB[result->HatIndex].Color1);
		result->HatColorsHSL.Color2 = UGTBFL::RGBToHSL(result->HatAsset->DefaultColorsRGB[result->HatIndex].Color2);
		result->HatColorsHSL.Color3 = UGTBFL::RGBToHSL(result->HatAsset->DefaultColorsRGB[result->HatIndex].Color3);
		result->HatColorsHSL = result->HatColorsHSL.ChangeTeamHue(teamHue, result->HatAsset->TeamColors[result->HatIndex % 10]);
	}

	return result;
}

void UCharacterDataAsset::PostLoad()
{
	Super::PostLoad();

	while (Accessories.Num() < 3)
		Accessories.Add(nullptr);
	while (Accessories.Num() > 3)
		Accessories.RemoveAt(3);
}

uint8 UCharacterDataAsset::GetBodyIndex()
{
	if (BodyAsset)
	{
		int32 index = UGTGameInstance::Instance->BodyTypes.Find(BodyAsset);
		if (index == INDEX_NONE)
		{
			UE_LOG(LogTemp, Warning, TEXT("%s was not in BodyTypes. Adding temporarily."), *BodyAsset->GetName());
			UGTGameInstance::Instance->BodyTypes.Add(BodyAsset);
			index = UGTGameInstance::Instance->BodyTypes.Find(BodyAsset);
		}
		return (uint8)index;
	}
	else
	{
		//if no BodyAsset, default to Basic (BodyTypes[0])
		return 0;
	}
}

uint8 UCharacterDataAsset::GetCombinedHatIndex()
{
	//no hat; don't care about asset
	if (HatIndex == 255) return 255;

	if (HatAsset)
	{
		int32 index = UGTGameInstance::Instance->HatTypes.Find(HatAsset);
		if (index == INDEX_NONE)
		{
			UE_LOG(LogTemp, Warning, TEXT("%s was not in HatTypes. Adding temporarily."), *HatAsset->GetName());
			UGTGameInstance::Instance->HatTypes.Add(HatAsset);
			index = UGTGameInstance::Instance->HatTypes.Find(HatAsset);
		}
		return (uint8)(index * 10 + HatIndex);
	}
	else
	{
		//if no HatAsset, default to HatTypes[0]
		return HatIndex;
	}

}

uint8 UCharacterDataAsset::GetLevel()
{
	return TotalXP / UGTGameInstance::Instance->XPPerLevel;
}

TArray<class UFeat*> UCharacterDataAsset::GetAvailableFeats()
{
	TArray<UFeat*> result;
	for (UFeat* feat : UGTGameInstance::Instance->Feats)
	{
		if(Feats.Contains(feat)) continue;
		
		bool bHavePreReqs = true;
		for (UFeat* preReq : feat->PreReqs)
		{
			if (!Feats.Contains(preReq))
			{
				bHavePreReqs = false;
				break;
			}
		}
		if(bHavePreReqs) result.Add(feat);
	}

	return result;
}

void UCharacterDataAsset::LearnFeat(class UFeat* feat)
{
	if (!feat)
	{
		UE_LOG(LogTemp, Error, TEXT("LearnFeat: feat is null"));
		return;
	}
	if (Feats.Contains(feat))
	{
		UE_LOG(LogTemp, Error, TEXT("LearnFeat: already know feat"));
		return;
	}

	Feats.Add(feat);
	if (RemainingXP < feat->XPCost)
	{
		UE_LOG(LogTemp, Warning, TEXT("LearnFeat: insufficient XP"));
		RemainingXP = 0;
	}
	else
	{
		RemainingXP -= feat->XPCost;
	}

	if(feat->bChassisFeat)
		ChassisFeatCount += 1;

	UE_LOG(LogTemp, Log, TEXT("LearnFeat: learned %s"), *feat->Name.ToString());
}

FCharacterData::FCharacterData()
{

}

FCharacterData::FCharacterData(class UCharacterDataAsset* data, int32 _id)
{
	Name = data->Name;
	ID = _id;

	SkinColorHSL = data->SkinColorHSL;
	HairIndex = data->HairIndex;
	HairColorHSL = data->HairColorHSL;
	BodyAsset = data->BodyAsset;
	BodyColorsHSL = data->BodyColorsHSL;
	HatAsset = data->HatAsset;
	HatIndex = data->HatIndex;
	HatColorsHSL = data->HatColorsHSL;

	/*Weapon = data->Weapon;
	Shield = data->Shield;
	Armor = data->Armor;
	Accessories = data->Accessories;*/

	TotalXP = data->TotalXP;
	RemainingXP = data->RemainingXP;
	for (UFeat* feat : data->Feats)
		Feats.Add(feat);
}

FCharacterData::~FCharacterData()
{

}

void FCharacterData::Randomize()
{

	HairIndex = FMath::RandRange(0, 9);
	if (HairIndex < 5)
	{
		Name = FText::FromStringTable("/Game/Data/Character/NamesMale", FString::Printf(TEXT("Male_%03d"), FMath::RandRange(0, 255)));
	}
	else
	{
		Name = FText::FromStringTable("/Game/Data/Character/NamesFemale", FString::Printf(TEXT("Female_%03d"), FMath::RandRange(0, 255)));
	}

	float k = FMath::FRandRange(0, 3);
	if (k < 1) SkinColorHSL = FLinearColor(20, .6 + k * (.375 - .6), .75 + k * (.6 - .75));
	else if (k < 2) SkinColorHSL = FLinearColor(20 + (k - 1) * (28 - 20), .375 + (k - 1) * (.538 - .375), .6 + (k - 1) * (.325 - .6));
	else SkinColorHSL = FLinearColor(28 + (k - 2) * (15 - 28), .538 + (k - 2) * (.5 - .538), .325 + (k - 2) * (.2 - .325));

	//SkinColorHSL = FLinearColor(20, .375f, .6f);
	HairColorHSL = FLinearColor(FMath::FRandRange(0, 60), FMath::FRandRange(0, 1),
		((FMath::RandRange(0, 9) == 0)? FMath::FRandRange(.8f, .9f) : FMath::FRandRange(.1f, SkinColorHSL.B)));

	BodyAsset = UGTGameInstance::Instance->BodyTypes[FMath::RandRange(0, UGTGameInstance::Instance->BodyTypes.Num() - 1)];
	BodyColorsHSL.Color0 = UGTBFL::RGBToHSL(BodyAsset->DefaultColorsRGB.Color0);
	BodyColorsHSL.Color1 = UGTBFL::RGBToHSL(BodyAsset->DefaultColorsRGB.Color1);
	BodyColorsHSL.Color2 = UGTBFL::RGBToHSL(BodyAsset->DefaultColorsRGB.Color2);
	BodyColorsHSL.Color3 = UGTBFL::RGBToHSL(BodyAsset->DefaultColorsRGB.Color3);

	HatAsset = UGTGameInstance::Instance->HatTypes[FMath::RandRange(0, UGTGameInstance::Instance->HatTypes.Num() - 1)];
	HatIndex = FMath::RandRange(0, 9);
	HatColorsHSL.Color0 = UGTBFL::RGBToHSL(HatAsset->DefaultColorsRGB[HatIndex].Color0);
	HatColorsHSL.Color1 = UGTBFL::RGBToHSL(HatAsset->DefaultColorsRGB[HatIndex].Color1);
	HatColorsHSL.Color2 = UGTBFL::RGBToHSL(HatAsset->DefaultColorsRGB[HatIndex].Color2);
	HatColorsHSL.Color3 = UGTBFL::RGBToHSL(HatAsset->DefaultColorsRGB[HatIndex].Color3);
}
