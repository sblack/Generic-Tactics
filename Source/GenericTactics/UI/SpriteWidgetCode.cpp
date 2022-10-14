// Fill out your copyright notice in the Description page of Project Settings.


#include "SpriteWidgetCode.h"
#include "Materials/MaterialInstanceDynamic.h"
//#include "../Appearance/ColorQuartetBFL.h"
#include "../Character/CharacterDataAsset.h"
#include "../Utility/GTBFL.h"
#include "Components/Image.h"


//void USpriteWidgetCode::NativePreConstruct()
//{
//	InitMats();
//}
//
//void USpriteWidgetCode::InitMats()
//{
//	BodyDMI = UMaterialInstanceDynamic::Create()
//}

void USpriteWidgetCode::SetFromData(class UCharacterDataAsset* Data, uint8 Team)
{
	SetSkinColor(UGTBFL::HSLtoRGB(Data->SkinColorHSL));
	SetHairColor(UGTBFL::HSLtoRGB(Data->HairColorHSL));
	SetHairIndex(Data->HairIndex);
	SetBodyType(Data->BodyAsset->Image);
	SetBodyColors(UColorQuartetBFL::HSLtoRGBQuartet(Data->BodyColorsHSL));
	SetHatType(Data->HatIndex, Data->HatAsset->Image, Data->HatAsset->ImageB);
	SetHatColors(UColorQuartetBFL::HSLtoRGBQuartet(Data->HatColorsHSL));
}

void USpriteWidgetCode::SetSkinColor(FLinearColor Value)
{
	BodyDMI->SetVectorParameterValue(TEXT("ColorSkin"), Value);
}

void USpriteWidgetCode::SetHairColor(FLinearColor Value)
{
	HairDMI->SetVectorParameterValue(TEXT("Color"), Value);
}

void USpriteWidgetCode::SetHairIndex(uint8 Index)
{
	if (Index == 255)
	{
		Hair->SetVisibility(ESlateVisibility::Hidden);
	}
	else
	{
		Hair->SetVisibility(ESlateVisibility::HitTestInvisible);
		HairDMI->SetScalarParameterValue(TEXT("Row"), Index % 5);
		HairDMI->SetScalarParameterValue(TEXT("Column"), Index / 5);
	}
}

void USpriteWidgetCode::SetBodyType(class UTexture2D* Image)
{
	BodyDMI->SetTextureParameterValue(TEXT("Texture"), Image);
}

void USpriteWidgetCode::SetBodyColors(struct FColorQuartet Colors)
{
	UColorQuartetBFL::SetColorQuartetParameter(BodyDMI, Colors);
}

void USpriteWidgetCode::SetHatType(uint8 Index, class UTexture2D* Image, class UTexture2D* ImageB)
{
	if (Index == 255)
	{
		HatBack->SetVisibility(ESlateVisibility::Hidden);
		HatFront->SetVisibility(ESlateVisibility::Hidden);
	}
	else
	{
		HatBack->SetVisibility(ESlateVisibility::HitTestInvisible);
		HatFront->SetVisibility(ESlateVisibility::HitTestInvisible);
		HatFrontDMI->SetTextureParameterValue(TEXT("Texture"), Image);
		HatBackDMI->SetTextureParameterValue(TEXT("Texture"), ImageB);
		HatFrontDMI->SetScalarParameterValue(TEXT("Row"), Index % 5);
		HatFrontDMI->SetScalarParameterValue(TEXT("Column"), Index / 5);
		HatBackDMI->SetScalarParameterValue(TEXT("Row"), Index % 5);
		HatBackDMI->SetScalarParameterValue(TEXT("Column"), Index / 5);
	}
}

void USpriteWidgetCode::SetHatColors(struct FColorQuartet Colors)
{
	UColorQuartetBFL::SetColorQuartetParameter(HatFrontDMI, Colors);
	UColorQuartetBFL::SetColorQuartetParameter(HatBackDMI, Colors);
}
