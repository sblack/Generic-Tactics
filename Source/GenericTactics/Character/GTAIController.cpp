// Fill out your copyright notice in the Description page of Project Settings.


#include "GTAIController.h"
#include "GTCharacter.h"
#include "CharacterDataAsset.h"
#include "AIObjective.h"
#include "../Combat/CombatManager.h"
#include "../Player/CameraPawn.h"
#include "../UI/GTHUDCode.h"



void AGTAIController::BeginPlay()
{
	Super::BeginPlay();
}

FORCEINLINE class AGTCharacter* AGTAIController::GetTacticsCharacter()
{
	return Cast<AGTCharacter>(GetCharacter());
}