// Fill out your copyright notice in the Description page of Project Settings.


#include "SpriteWidgetCode.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "../Character/CharacterDataAsset.h"
#include "../Utility/GTBFL.h"

void USpriteWidgetCode::SetFromData(class UCharacterDataAsset* Data, uint8 Team)
{

}

void USpriteWidgetCode::SetSkinColor(FLinearColor Value)
{
	BodyDMI->SetVectorParameterValue(TEXT("ColorSkin"), Value);
}
