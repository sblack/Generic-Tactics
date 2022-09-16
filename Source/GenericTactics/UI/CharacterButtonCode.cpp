// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterButtonCode.h"
#include "SpriteWidgetCode.h"
#include "../Player/GTSaveGame.h"
#include "Components/Button.h"
#include "Components/Image.h"

void UCharacterButtonCode::Select_Implementation()
{
	Frame->SetColorAndOpacity(FLinearColor(0, .5f, 1));
	bIsSelected = true;

	//TODO: limit to out-of-combat
	SpriteWidget->SetFromData(CharacterData, 0);
}

void UCharacterButtonCode::Deselect_Implementation()
{
	Frame->SetColorAndOpacity(FLinearColor(1, 1, 1));
	bIsSelected = false;

	//TODO: limit to out-of-combat
	SpriteWidget->SetFromData(CharacterData, 0);
	UGTSaveGame::Instance->SaveCharacter(CharacterData);
}

void UCharacterButtonCode::Place_Implementation(class AGTCharacter* chara)
{
	Character = chara;
	Frame->SetVisibility(ESlateVisibility::HitTestInvisible);
	Frame->SetColorAndOpacity(FLinearColor(1, .5f, 0));
	Button0->SetBackgroundColor(FLinearColor(1, .5f, 0));
	bIsPlaced = true;
	bIsSelected = false;
}

void UCharacterButtonCode::Unplace_Implementation()
{
	Character = nullptr;
	Button0->SetBackgroundColor(FLinearColor(1, 1, 1));
	bIsPlaced = false;
}
