// Fill out your copyright notice in the Description page of Project Settings.


#include "GTGameInstance.h"
#include "Feats/Feat.h"
#include "Items/Item.h"
#include "Items/ItemEquipment.h"
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
	FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
	TArray<FAssetData> AssetData;

	UE_LOG(LogTemp, Log, TEXT("   LOADING FEATS"));
	FARFilter Filter;
	Filter.ClassNames.Add(UFeat::StaticClass()->GetFName());
	for (int i = 0; i < FeatPaths.Num(); i++)
	{
		FName path = FName(*FeatPaths[i], FNAME_Find);
		if (path != NAME_None)
			Filter.PackagePaths.Add(path);
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Path %s not found."), *FeatPaths[i]);
		}
	}
	Filter.bRecursivePaths = true;
	AssetRegistryModule.Get().GetAssets(Filter, AssetData);
	for (int i = 0; i < AssetData.Num(); i++)
	{
		if (AssetData[i].AssetClass == UFeat::StaticClass()->GetFName())
		{
			UFeat* feat = Cast<UFeat>(AssetData[i].GetAsset());
			Feats.Add(feat);
			for (UFeat* prereq : feat->PreReqs)
			{
				prereq->DerivedFeats.Add(feat);
			}
		}
		//UE_LOG(LogTemp, Log, TEXT("%s: %s"), *AssetData[i].AssetName.ToString(), *AssetData[i].AssetClass.ToString());
	}
	UE_LOG(LogTemp, Log, TEXT("   %d Feats Loaded"), Feats.Num());


	UE_LOG(LogTemp, Log, TEXT("   LOADING ITEMS"));
	AssetData.Empty();
	Filter = FARFilter();
	Filter.ClassNames.Add(UItem::StaticClass()->GetFName());
	for (int i = 0; i < ItemPaths.Num(); i++)
	{
		FName path = FName(*ItemPaths[i], FNAME_Find);
		if (path != NAME_None)
			Filter.PackagePaths.Add(path);
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Path %s not found."), *ItemPaths[i]);
		}
	}
	Filter.bRecursivePaths = true;
	Filter.bRecursiveClasses = true;
	AssetRegistryModule.Get().GetAssets(Filter, AssetData);

	for (int i = 0; i < AssetData.Num(); i++)
	{
		UItemEquipment* equip = Cast<UItemEquipment>(AssetData[i].GetAsset());
		if(equip)
			Equipment.Add(equip);
		//TODO: else, add to list of non-equipment items
		//Items.Add(Cast<UItem>(AssetData[i].GetAsset()));
		//UE_LOG(LogTemp, Log, TEXT("%s: %s"), *AssetData[i].AssetName.ToString(), *AssetData[i].AssetClass.ToString());
	}
}
