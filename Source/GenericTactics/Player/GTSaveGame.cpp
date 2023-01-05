// Fill out your copyright notice in the Description page of Project Settings.


#include "GTSaveGame.h"
#include "Engine.h"

UGTSaveGame* UGTSaveGame::Instance;

UGTSaveGame::UGTSaveGame()
{
	Instance = this;
	UE_LOG(LogTemp, Log, TEXT("GT SaveGame constructed"));
}

void UGTSaveGame::SaveCharacter(class UCharacterDataAsset* character)
{
	if (character == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("Attempt to save null character"));
		return;
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("Saving %s %d"), *character->Name.ToString(), character->ID);
	}


	if (character->ID == -1) //new character; add to list
	{
		if (Characters.Num() == 0)
			Characters.Add(FCharacterData(character, 0));
		else
			Characters.Add(FCharacterData(character, Characters.Last().ID + 1));

		UE_LOG(LogTemp, Log, TEXT("New character %s saved"), *character->Name.ToString());
	}
	else //changed character; override in list
	{
		for (int i = 0; i < Characters.Num(); i++)
		{
			/*if (!Characters[i]) //can't null check structs
			{
				UE_LOG(LogTemp, Error, TEXT("Character[%d] is null"), i);
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Character[%d] is null"), i));
				return;
			}*/
			if (Characters[i].ID == character->ID)
			{

				FText oldName = Characters[i].Name;

				/*if (&oldName == nullptr)
				{
					UE_LOG(LogTemp, Error, TEXT("WHYYYYYYYYY"));
					return;
				}*/

				Characters[i] = FCharacterData(character, character->ID);

				//DISABLING UNTIL A SOLUTION IS FOUND
				//if (character->Name.EqualTo(oldName)) //WHAT PROBLEM DOES UE4 HAVE WITH THIS LINE?!
				{
					UE_LOG(LogTemp, Log, TEXT("Character %s modified"), *character->Name.ToString());
				}
				/*else
				{
					UE_LOG(LogTemp, Log, TEXT("Old character %s saved as %s"), *oldName.ToString(), *character->Name.ToString());
				}*/
				return;
			}
		}
		UE_LOG(LogTemp, Warning, TEXT("New character %s has non -1 ID"), *character->Name.ToString());

		if (Characters.Num() == 0)
			Characters.Add(FCharacterData(character, 0));
		else
			Characters.Add(FCharacterData(character, Characters.Last().ID + 1));

		UE_LOG(LogTemp, Log, TEXT("New character %s saved"), *character->Name.ToString());
	}
}

//Incomplete
// issue: newly created characters would be missed if they haven't been saved yet
//void UGTSaveGame::SaveCharacters()
//{
//	for (int i = 0; i < Characters.Num(); i++)
//	{
//		UCharacterDataAsset* asset = *AssetCache.Find(Characters[i].ID);
//		
//		if (!asset)
//		{
//			UE_LOG(LogTemp, Warning, TEXT("%s (ID %d) has no cached asset?"), *Characters[i].Name.ToString(), Characters[i].ID);
//		}
//	}
//}

void UGTSaveGame::DeleteCharacter(class UCharacterDataAsset* character)
{
	if (!character || character->ID == -1 || Characters.Num() == 0)
		return;

	/*for (int i = 0; i < Characters.Num(); i++)
	{
		if (Characters.Last(i).ID == character->ID)
		{
			Characters.RemoveAt(Characters.Num() - 1 - i);
			return;
		}
	}*/
	for (int i = 0; i < Characters.Num(); i++)
	{
		if (Characters[i].ID == character->ID)
		{
			Characters.RemoveAt(i);
			UE_LOG(LogTemp, Log, TEXT("%s deleted"), *character->Name.ToString());
			return;
		}
	}
	UE_LOG(LogTemp, Warning, TEXT("%s not found for deletion"), *character->Name.ToString());
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("%s not found for deletion"), *character->Name.ToString()));
}

class UCharacterDataAsset* UGTSaveGame::GetCharacterByID(int32 ID)
{
	if (AssetCache.Contains(ID))
		return *AssetCache.Find(ID);

	for (int i = 0; i < Characters.Num(); i++)
	{
		if (Characters[i].ID == ID)
		{
			UCharacterDataAsset* asset = UCharacterDataAsset::FromSave(Characters[i]);
			AssetCache.Add(ID, asset);
			return asset;
		}
	}
	UE_LOG(LogTemp, Error, TEXT("No character found with ID %d"), ID);
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("No character found with ID %d"), ID));
	return nullptr;
}

TArray<class UCharacterDataAsset*> UGTSaveGame::CharacterAssets()
{
	//if AssetCache is missing anyone, add them; otherwise, does nothing
	for (int i = 0; i < Characters.Num(); i++)
		GetCharacterByID(Characters[i].ID);

	TArray<class UCharacterDataAsset*> result;
	AssetCache.GenerateValueArray(result);
	return result;
}
