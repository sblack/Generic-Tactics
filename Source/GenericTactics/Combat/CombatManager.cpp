// Fill out your copyright notice in the Description page of Project Settings.


#include "CombatManager.h"
#include <EngineGlobals.h>
#include <Runtime/Engine/Classes/Engine/Engine.h>
#include "Kismet/GameplayStatics.h"
#include "../Character/GTAIController.h"
#include "../Character/GTCharacter.h"
#include "../Character/HumanBase.h"
#include "../Character/StatsBlock.h"
#include "../Movement/NavGrid.h"
#include "../Player/CameraPawn.h"
//#include "../Player/GTPlayerController.h"
#include "../UI/GTHUDCode.h"
#include "../UI/InitiativeTrackCode.h"
#include "PaperFlipbookComponent.h"
//#include "Action.h"
#include "ActionAttack.h"
//#include "ShapeBase.h"
#include "Kismet/KismetMathLibrary.h"

UCombatManager* UCombatManager::Instance;
UWorld* UCombatManager::BackupWorld;

void UCombatManager::AdvanceActionQueue()
{
	while (ActionQueue.Num() > 0)
	{
		FActionData action = ActionQueue[0];
		ActionQueue.RemoveAt(0);

		if (action.Source) //&& action.Actor is not dead
		{
			//action.Action->Perform(action.Actor, action.Location);
			return;
		}
	}

	bPerformingActions = false;
}

//required in order to spawn things in BP
class UWorld* UCombatManager::GetWorld() const
{
	return (!HasAnyFlags(RF_ClassDefaultObject) && !GetOuter()->HasAnyFlags(RF_BeginDestroyed) && !GetOuter()->IsUnreachable() ? CastChecked<UGameInstance>(GetOuter())->GetWorld() : BackupWorld);
}

TArray<class AGTCharacter*> UCombatManager::GetTeam(bool bGetParty)
{
	if(bGetParty) return Instance->PartyCharacters;
	else return Instance->EnemyCharacters;
}

UCombatManager::UCombatManager()
{
	Instance = this;
}

void UCombatManager::StartCombat()
{
	UE_LOG(LogTemp, Log, TEXT("START COMBAT"));
	Instance->bPreCombat = false;
	ACameraPawn::Instance->RevealMap();

	TArray<AActor*> temp;
	UGameplayStatics::GetAllActorsOfClass(GEngine->GameViewport->GetWorld(), AGTCharacter::StaticClass(), temp);
	if (temp.Num() == 0)
	{
		UE_LOG(LogTemp, Error, TEXT("0 GTCharacters found"));
		return;
	}
	//UE_LOG(LogTemp, Log, TEXT("temp.Num() = %d"), temp.Num());
	//UE_LOG(LogTemp, Log, TEXT("iQueue.Num() = %d"), Instance->InitiativeQueue.Num());
	Instance->InitiativeQueue.Empty(temp.Num());
	Instance->EnemyCharacters.Empty();
	Instance->PartyCharacters.Empty();
	UGTGameInstance::Instance->RecentParty.Empty();
	for (int i = 0; i < temp.Num(); i++)
	{
		bool bInserted = false;
		AGTCharacter* gtc = Cast<AGTCharacter>(temp[i]);
		for (int j = 0; j < Instance->InitiativeQueue.Num(); j++)
		{
			if (gtc->GetInitiative() > Instance->InitiativeQueue[j]->GetInitiative())
			{
				Instance->InitiativeQueue.Insert(gtc, j);
				bInserted = true;
				break;
			}
		}
		if (!bInserted)
			Instance->InitiativeQueue.Add(gtc);

		if (gtc->ActorHasTag(FName("Party")))
		{
			Instance->PartyCharacters.Add(gtc);
			UE_LOG(LogTemp, Log, TEXT("%s added to PartyCharacters"), *gtc->GetName());
			UGTGameInstance::Instance->RecentParty.Add(Cast<AHumanBase>(gtc)->CharacterData);
			UGTGameInstance::Instance->RecentPartyIDs.Add(Cast<AHumanBase>(gtc)->CharacterData->ID, true);
		}
		else
		{
			Instance->EnemyCharacters.Add(gtc); //TODO: ally characters would need separate array
			UE_LOG(LogTemp, Log, TEXT("%s added to EnemyCharacters"), *gtc->GetName());
		}
	}

	for (int i = 0; i < Instance->InitiativeQueue.Num(); i++)
	{
		UGTHUDCode::Instance->GetInitiativeTrack()->AddMarker(Instance->InitiativeQueue[i]);
	}

	Instance->InitiativeQueue[0]->BeginTurn();
	Instance->InitiativeIndex = 0;

	BackupWorld = nullptr; //make sure we're not using an old BackupWorld
	if (Instance->GetWorld())
	{
		UE_LOG(LogTemp, Log, TEXT("GetWorld succeeded"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("GetWorld failed; trying with backup"));
		BackupWorld = Instance->PartyCharacters[0]->GetWorld();
		if (Instance->GetWorld())
		{
			UE_LOG(LogTemp, Log, TEXT("backup GetWorld succeeded"));
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("backup GetWorld failed"));
		}
	}
}

void UCombatManager::StartPreCombat()
{
	if(Instance)
		Instance->bPreCombat = true;
	else
	{
		UE_LOG(LogTemp, Error, TEXT("No CombatManager, cannot start precombat"));
	}
}

void UCombatManager::AdvanceInitiative()
{
	Instance->InitiativeIndex += 1;
	if (Instance->InitiativeIndex >= Instance->InitiativeQueue.Num())
	{
		UE_LOG(LogTemp, Log, TEXT("Finished Initiative Queue, returning to start"));
		Instance->InitiativeIndex = 0;
	}

	UGTHUDCode::Instance->GetInitiativeTrack()->AdvanceInitiative();
	Instance->InitiativeQueue[Instance->InitiativeIndex]->BeginTurn();
}

void UCombatManager::InitiateActionTarget(class UAction* action, class AGTCharacter* attacker, TScriptInterface<ITargetableInterface> target)
{
	if (!attacker || !target)
	{
		UE_LOG(LogTemp, Error, TEXT("IAT: missing attacker or target"));
		return;
	}

	Instance->ActionQueue.Empty();
	FActionData act;
	act.Source = attacker;
	act.Action = action;
	act.Location = target->GetLocation() - attacker->GetActorLocation();
	Instance->ActionQueue.Emplace(act);

	//TODO: determine interrupts and reactions
	float attackInitiative = attacker->RollInitiative() + 10;
	if (target->AsActor()->IsA<AGTCharacter>())
	{
		AGTCharacter* targetActor = Cast<AGTCharacter>(target->AsActor());
		/*if (targetActor->CanReact())
		{
			FActionData react;
			react.Actor = targetActor;
			react.Action = Instance->DefaultAttack;
			react.RelativeLocation = attacker->GetActorLocation() - targetActor->GetActorLocation();

			float reactInitiative = targetActor->RollInitiative();
			if (reactInitiative > attackInitiative)
			{
				Instance->InterruptQueue.Emplace(react);
			}
			else
			{
				Instance->ReactionQueue.Emplace(react);
			}
		}*/

		targetActor->SetActorRotation(UKismetMathLibrary::MakeRotFromX(attacker->GetActorLocation() - targetActor->GetActorLocation()));
	}

	Instance->bPerformingActions = true;
	//action->Perform(attacker, target->GetLocation() - attacker->GetActorLocation());
	//UGTHUDCode::Instance->ShowAttackText(NSLOCTEXT("Combat", "UIAttack", "Attack"));
}

void UCombatManager::InitiateActionLocation(class UAction* action, class AGTCharacter* attacker, FVector location)
{
	if (!attacker)
	{
		UE_LOG(LogTemp, Error, TEXT("IAL: missing attacker"));
		return;
	}

	Instance->ActionQueue.Empty();
	FActionData act;
	act.Source = attacker;
	act.Action = action;
	act.Location = location - attacker->GetActorLocation();
	Instance->ActionQueue.Emplace(act);

	//TODO: determine interrupts and reactions

	Instance->bPerformingActions = true;
	//action->Perform(attacker, location - attacker->GetActorLocation());
	//UGTHUDCode::Instance->ShowAttackText(NSLOCTEXT("Combat", "UIAttack", "Attack"));
}

void UCombatManager::InitiatePreparedAction(class AGTCharacter* attacker)
{
	ANavGrid::Instance->ShowMoveRange(nullptr);
	InitiateActionLocation(Instance->PreppedAction, attacker, Instance->PreppedTarget);
}

//void UCombatManager::InterruptAction()
//{
//	UTacticsHUDCode::Instance->ClearAttackText();
//	if (!Instance->bPerformingActions)
//	{
//		UE_LOG(LogTemp, Warning, TEXT("No action to interrupt"));
//		return;
//	}
//
//	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Interrupt?"));
//
//	if (Instance->bInterrupting || Instance->InterruptQueue.Num() == 0)
//	{
//		return;
//	}
//
//	UE_LOG(LogTemp, Log, TEXT("Interrupt!"));
//	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Interrupt!"));
//
//	Instance->bInterrupting = true;
//
//	FActionData interrupt = Instance->InterruptQueue[0];
//	Instance->InterruptQueue.RemoveAt(0);
//
//	interrupt.Action->Perform(interrupt.Actor, interrupt.RelativeLocation);
//	ActingController()->GetTacticsCharacter()->GetSprite()->SetPlayRate(0);
//}

void UCombatManager::PrepareDirectAction(class UActionDirect* action)
{
	if (action == nullptr)
	{
		Instance->PreppedAction = nullptr;
		Instance->AreaOfEffect.Empty();
		//AGTPlayerController::Instance->UpdateFinalAP(0);
	}
	else
	{
		Instance->PreppedAction = action;
		//AGTPlayerController::Instance->UpdateFinalAP(action->APCost);
	}
}

void UCombatManager::UpdateAreaOfEffect(FVector source, FVector target)
{
	if (Instance->PreppedAction == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("Can't update AoE without Action"));
		return;
	}

	Instance->PreppedTarget = target;

	/*if (Instance->PreppedAction->Shape == nullptr)
	{
		Instance->AreaOfEffect.Empty(1);
		Instance->AreaOfEffect.Add(target);
	}
	else
	{
		Instance->AreaOfEffect = Instance->PreppedAction->Shape->GetTiles(target, source);
	}

	ANavGrid::Instance->ShowTargeting(source, Instance->PreppedAction->MaxRange);*/
}

ESuccessLevel UCombatManager::RollAttack(class UActionAttack* action, TScriptInterface<IActionSourceInterface> source, TScriptInterface<ITargetableInterface> target)
{
	if (Instance)
	{
		//TODO: more in-depth calculations (eg acc bonus based on target, def bonus based on attacker, etc)
		return Instance->AttackRoll(source->GetAccuracy(action->AttackType), target->GetDefense(action->DefenseType));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("No CombatManager"));
		return ESuccessLevel::Hit;
	}
}

void UCombatManager::ResetDetection(class AGTCharacter* mover)
{
	Instance->AwareEnemies.Empty();
	//UnawareEnemies.Empty();

	if (mover->IsPartyCharacter())
	{
		//TODO: sneak/unaware
		for (int i = 0; i < Instance->EnemyCharacters.Num(); i++)
		{
			Instance->AwareEnemies.Add(Instance->EnemyCharacters[i]);
		}
	}
	else
	{
		//TODO: sneak/unaware
		for (int i = 0; i < Instance->PartyCharacters.Num(); i++)
		{
			Instance->AwareEnemies.Add(Instance->PartyCharacters[i]);
		}
	}

	CheckDetection(mover);
}

void UCombatManager::CheckDetection(class AGTCharacter* mover)
{
	//TODO: unaware

	for (int i = 0; i < Instance->AwareEnemies.Num(); i++)
	{
		if (ANavGrid::Instance->GetDistance(mover->GetActorLocation() - Instance->AwareEnemies[i]->GetActorLocation()) < Instance->AwareEnemies[i]->Stats->DetectionRadius + .5) //.5 for rounding
		{
			Instance->AwareEnemies[i]->TurnToFace(mover->GetActorLocation());
		}
	}
}

void UCombatManager::EndCurrentTurn()
{
	Instance->InitiativeQueue[Instance->InitiativeIndex]->EndTurn();
}

void UCombatManager::RemoveCharacter(class AGTCharacter* character)
{
	UE_LOG(LogTemp, Log, TEXT("Removing %s from combat"), *character->GetDisplayName().ToString());
	if (Instance->InitiativeQueue[Instance->InitiativeIndex] == character)
	{
		UE_LOG(LogTemp, Warning, TEXT("   Character removed on its own turn; may need handling"));
		character->EndTurn();
	}
	int index = Instance->InitiativeQueue.Find(character);
	if (index < Instance->InitiativeIndex)
		Instance->InitiativeIndex -= 1;
	Instance->InitiativeQueue.RemoveAt(index);
	UGTHUDCode::Instance->GetInitiativeTrack()->RemoveMarker(character);
	if(character->IsPartyCharacter())
	{
		Instance->PartyCharacters.Remove(character);
		character->OnDeath();
	}
	else
	{
		Instance->EnemyCharacters.Remove(character);
		if (Instance->EnemyCharacters.Num() == 0)
		{
			UGTHUDCode::Instance->EndCombat(true);
		}
	}

}

void UCombatManager::AddToDeathQueue(class AGTCharacter* character)
{
	if (!Instance->DeathQueue.Contains(character))
	{
		Instance->DeathQueue.Add(character);
		UE_LOG(LogTemp, Log, TEXT("%s added to death queue"), *character->GetDisplayName().ToString());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("%s is already in death queue"), *character->GetDisplayName().ToString());
	}
}

void UCombatManager::CheckDeathQueue()
{
	if (Instance->DeathQueue.Num() > 0)
	{
		if (!ACameraPawn::Instance->IsOnScreen(Instance->DeathQueue[0]->GetActorLocation()))
			ACameraPawn::Instance->AttachCamera(Instance->DeathQueue[0]);
		if (Instance->DeathQueue[0]->IsPartyCharacter())
		{
			Instance->bCheckDefeat = true;
			//TODO: Character::OnDying()
		}
		else
			Instance->DeathQueue[0]->OnDeath();
		Instance->DeathQueue.RemoveAt(0);
	}
	else
	{
		if (Instance->bCheckDefeat)
		{
			bool bAllDown = true;
			for (AGTCharacter* chara : Instance->PartyCharacters)
			{
				if (!chara->GetIsDead())
				{
					bAllDown = false;
					break;
				}
			}

			if (bAllDown)
			{
				UGTHUDCode::Instance->EndCombat(false);
				return;
			}
		}
		Instance->bCheckDefeat = false;

		AGTCharacter* character = Cast<AGTCharacter>(Instance->InitiativeQueue[Instance->InitiativeIndex]->AsActor());
		if(character)
		{
			ACameraPawn::Instance->AttachCamera(character);
			character->AdvanceAI();
		}
		else
			Instance->InitiativeQueue[Instance->InitiativeIndex]->EndTurn(); //non-character sources only do one thing, and it's been done if we're here, so end turn
	}
}
