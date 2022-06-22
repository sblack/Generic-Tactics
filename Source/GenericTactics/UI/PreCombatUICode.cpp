// Fill out your copyright notice in the Description page of Project Settings.


#include "PreCombatUICode.h"
#include "../Combat/CombatManager.h"

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
