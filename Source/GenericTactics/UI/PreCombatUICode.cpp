// Fill out your copyright notice in the Description page of Project Settings.


#include "PreCombatUICode.h"
#include "CharacterButtonCode.h"
#include "../Combat/CombatManager.h"
#include "../Movement/NavGrid.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"

UPreCombatUICode* UPreCombatUICode::Instance;

void UPreCombatUICode::NativeConstruct()
{
	Super::NativeConstruct();

	Instance = this;
	UCombatManager::StartPreCombat();
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
	PartyCount->SetText(FText::FromString(FString::Printf(TEXT("%d/%d"), Count, 16)));
	if (Count == 0 || Count > 16)
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

void UPreCombatUICode::ClickVoid_Implementation()
{
	if (PlacedCharacter)
	{
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

	if (PlacedCharacter)
	{
		PlacedCharacter->SetActorLocation(ANavGrid::Instance->AlignToGrid(location + FVector(0,0,80)));
		SelectedButton->Place(PlacedCharacter);
		PlacedCharacter = nullptr;
	}
	else
	{
		FTransform transform = FTransform(ANavGrid::Instance->AlignToGrid(location + FVector(0, 0, 80)));
		PlacedCharacter = GetWorld()->SpawnActorDeferred<AGTCharacter>(AGTCharacter::StaticClass(), transform);
		if(PlacedCharacter)
		{
			PlacedCharacter->CharacterData = SelectedButton->CharacterData;
			UGameplayStatics::FinishSpawningActor(PlacedCharacter, transform);
			SelectedButton->Place(PlacedCharacter);
			PlacedParty.Add(PlacedCharacter);
			Count++;
			CountChanged();
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
	if (SelectedButton)
	{
		FakeCharActor->SetActorLocation(location);
		FakeCharActor->SetActorHiddenInGame(false);
		ValidText->SetText(FText::FromString(TEXT("Valid")));
	}
}
