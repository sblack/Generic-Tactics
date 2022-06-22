// Copyright Epic Games, Inc. All Rights Reserved.


#include "GenericTacticsGameModeBase.h"
#include "Player/GTPlayerController.h"
#include "UObject/ConstructorHelpers.h"

#if WITH_EDITOR
//#include "Utility/StatsCustomization.h"
//#include "PropertyEditorModule.h"
#endif

void AGenericTacticsGameModeBase::BeginPlay()
{
	UE_LOG(LogTemp, Log, TEXT("BeginPlay GameMode"));
	Super::BeginPlay();
}

AGenericTacticsGameModeBase::AGenericTacticsGameModeBase()
{
	// use our custom PlayerController class
	PlayerControllerClass = AGTPlayerController::StaticClass();

#if WITH_EDITOR
	//FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
	////Custom detail views
	////PropertyModule.RegisterCustomClassLayout("SOActor", FOnGetDetailCustomizationInstance::CreateStatic(&SOActorCustomization::MakeInstance));

	//PropertyModule.RegisterCustomPropertyTypeLayout("StatsInt", FOnGetPropertyTypeCustomizationInstance::CreateStatic(&FStatsIntCustomization::MakeInstance));
	//PropertyModule.RegisterCustomPropertyTypeLayout("StatsFloat", FOnGetPropertyTypeCustomizationInstance::CreateStatic(&FStatsFloatCustomization::MakeInstance));
#endif
}
