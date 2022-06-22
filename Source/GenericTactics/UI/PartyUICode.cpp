// Fill out your copyright notice in the Description page of Project Settings.


#include "PartyUICode.h"

UPartyUICode* UPartyUICode::Instance;
FLinearColor UPartyUIPanelCode::PickedColor;
bool UPartyUIPanelCode::bHavePickedColor;

void UPartyUICode::NativeConstruct()
{
	Super::NativeConstruct();

	UPartyUIPanelCode::bHavePickedColor = false;
}

void UPartyUICode::NativeDestruct()
{
	if (Instance == this)
		Instance = nullptr;

	Super::NativeDestruct();
}

void UPartyUICode::NativePreConstruct()
{
	Super::NativePreConstruct();
	Instance = this;
	UE_LOG(LogTemp, Log, TEXT("Instance set"));
}
