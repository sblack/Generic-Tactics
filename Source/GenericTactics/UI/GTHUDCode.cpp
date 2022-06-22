// Fill out your copyright notice in the Description page of Project Settings.


#include "GTHUDCode.h"

UGTHUDCode* UGTHUDCode::Instance;

void UGTHUDCode::NativeConstruct()
{
	Super::NativeConstruct();

	Instance = this;
}

void UGTHUDCode::NativeDestruct()
{
	if (Instance == this)
		Instance = nullptr;

	Super::NativeDestruct();
}

