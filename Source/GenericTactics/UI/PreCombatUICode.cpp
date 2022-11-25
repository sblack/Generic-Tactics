// Fill out your copyright notice in the Description page of Project Settings.


#include "PreCombatUICode.h"
#include "CharacterButtonCode.h"
#include "../Combat/CombatManager.h"
#include "../Movement/NavGrid.h"
#include "../Player/CameraPawn.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"

UPreCombatUICode* UPreCombatUICode::Instance;

void UPreCombatUICode::NativeConstruct()
{
	Super::NativeConstruct();

	Instance = this;
	UCombatManager::StartPreCombat();
	if(ANavGrid::Instance)
	{
		StartArea = ANavGrid::Instance->GetStartArea();
		if (StartArea.Num() < MaxCount)
		{
			UE_LOG(LogTemp, Log, TEXT("Start Area is smaller than MaxCount; reducing MaxCount"));
			MaxCount = StartArea.Num();
			CountChanged();
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("NavGrid does not exist yet"));
	}
}

void UPreCombatUICode::NativeDestruct()
{
	if (Instance == this)
		Instance = nullptr;

	Super::NativeDestruct();
}

void UPreCombatUICode::CountChanged_Implementation()
{
	//TODO: get proper max party
	PartyCount->SetText(FText::FromString(FString::Printf(TEXT("%d/%d"), Count, MaxCount)));
	if (Count > 0 && Count <= MaxCount)
	{
		PartyCount->SetColorAndOpacity(FSlateColor(FLinearColor(1, 1, 1)));
		StartButton->SetIsEnabled(true);
	}
	else
	{
		PartyCount->SetColorAndOpacity(FSlateColor(FLinearColor(1, 0, 0)));
		StartButton->SetIsEnabled(false);
	}
}

void UPreCombatUICode::AutoDeploy()
{
	if (StartArea.Num() == 0)
	{
		if (ANavGrid::Instance)
		{
			StartArea = ANavGrid::Instance->GetStartArea();
			if (StartArea.Num() < MaxCount)
			{
				UE_LOG(LogTemp, Log, TEXT("Start Area is smaller than MaxCount; reducing MaxCount"));
				MaxCount = StartArea.Num();
				CountChanged();
			}
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("NavGrid still does not exist yet"));
			return;
		}
	}

	//already at or over max; nothing to do
	if(Count >= MaxCount) return;

	for (int i = 0; i < StartArea.Num(); i++)
	{
		if(StartArea[i]->Occupant) continue;

		UCharacterButtonCode* toBePlaced = nullptr;
		for (int j = 0; j < PartyButtons.Num(); j++)
		{
			if (!PartyButtons[j]->GetIsPlaced())
			{
				toBePlaced = PartyButtons[j];
				break;
			}
		}
		if (!toBePlaced)
		{
			//no unplaced characters; done
			break;
		}

		AGTCharacter* toBeChar = SpawnCharacter(StartArea[i]->Location + FVector(0, 0, 80), toBePlaced->CharacterData);
		if (toBeChar)
		{
			toBePlaced->Place(toBeChar);
			PlacedParty.Add(toBeChar);
			ANavGrid::AddActorToGrid(toBeChar);
			Count++;

			if(Count == MaxCount)
				break;
		}
	}
	CountChanged();
}

void UPreCombatUICode::ClickVoid_Implementation()
{
	if (PlacedCharacter) //if character selected and placed, remove character
	{
		ANavGrid::RemoveActorFromGrid(PlacedCharacter);
		PlacedParty.Remove(PlacedCharacter);
		SelectedButton->Unplace();
		PlacedCharacter->Destroy();
		PlacedCharacter = nullptr;
		Count--;
		CountChanged();
	}
	if (SelectedButton)
	{
		SelectedButton->Deselect();
		SelectedButton = nullptr;
	}
}

void UPreCombatUICode::ClickCharacter_Implementation(class AGTCharacter* character)
{
	//TODO
}

void UPreCombatUICode::ClickLocation_Implementation(FVector location)
{
	if (!SelectedButton || FakeCharActor->IsHidden())
		return;

	if (PlacedCharacter) //if selected character is already placed, move them
	{
		ANavGrid::RemoveActorFromGrid(PlacedCharacter);
		PlacedCharacter->SetActorLocation(ANavGrid::Instance->AlignToGrid(location + FVector(0,0,80)));
		SelectedButton->Place(PlacedCharacter);
		ANavGrid::AddActorToGrid(PlacedCharacter);
		PlacedCharacter = nullptr;
	}
	else //otherwise, spawn character
	{
		PlacedCharacter = SpawnCharacter(ANavGrid::Instance->AlignToGrid(location + FVector(0, 0, 80)), SelectedButton->CharacterData);
		if(PlacedCharacter)
		{
			SelectedButton->Place(PlacedCharacter);
			PlacedParty.Add(PlacedCharacter);
			ANavGrid::AddActorToGrid(PlacedCharacter);
			Count++;
			CountChanged();
			PlacedCharacter = nullptr;
		}
	}
	SelectedButton = nullptr;

}

void UPreCombatUICode::HoverVoid_Implementation()
{
	ValidText->SetText(FText::FromString(TEXT("NOT VALID")));
	if(FakeCharActor)
		FakeCharActor->SetActorHiddenInGame(true);
}

void UPreCombatUICode::HoverCharacter_Implementation(class AGTCharacter* character)
{
	if (SelectedButton)
	{
		ValidText->SetText(FText::FromString(TEXT("NOT VALID")));
		if (FakeCharActor)
			FakeCharActor->SetActorHiddenInGame(true);
	}
	else
	{
		//TODO
	}
}

void UPreCombatUICode::HoverLocation_Implementation(FVector location)
{
	if (StartFacing.Roll == 90) //ToOrientationRotator sets Roll to 0, so we know when it has or hasn't been done
	{
		StartFacing = (ACameraPawn::Instance->GetActorForwardVector() * FVector(1, 1, 0)).ToOrientationRotator();
	}
	if (SelectedButton)
	{
		FakeCharActor->SetActorLocation(location);
		FakeCharActor->SetActorRotation(StartFacing);
		FakeCharActor->SetActorHiddenInGame(false);
		ValidText->SetText(FText::FromString(TEXT("Valid")));
	}
}
