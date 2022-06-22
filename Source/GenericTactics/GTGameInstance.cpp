// Fill out your copyright notice in the Description page of Project Settings.


#include "GTGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "AssetRegistryModule.h"
#include "ARFilter.h"

UGTGameInstance* UGTGameInstance::Instance;

void UGTGameInstance::Init()
{
	Super::Init();
	Instance = this;

	LoadData();
}

void UGTGameInstance::Shutdown()
{
	UE_LOG(LogTemp, Log, TEXT("Shutting Down"));

	Super::Shutdown();
}

void UGTGameInstance::LoadData()
{

}
