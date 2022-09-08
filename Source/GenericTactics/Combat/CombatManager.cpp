// Fill out your copyright notice in the Description page of Project Settings.


#include "CombatManager.h"
#include <EngineGlobals.h>
#include <Runtime/Engine/Classes/Engine/Engine.h>
#include "Kismet/GameplayStatics.h"
#include "../Character/GTAIController.h"
#include "../Character/GTCharacter.h"
#include "../Player/CameraPawn.h"
//#include "../Player/GTPlayerController.h"
#include "../UI/GTHUDCode.h"
#include "PaperFlipbookComponent.h"
//#include "Action.h"
#include "ActionAttack.h"
//#include "ShapeBase.h"
#include "Kismet/KismetMathLibrary.h"

UCombatManager* UCombatManager::Instance;

void UCombatManager::AdvanceActionQueue()
{
	while (ActionQueue.Num() > 0)
	{
		FActionData action = ActionQueue[0];
		ActionQueue.RemoveAt(0);

		if (action.Actor) //&& action.Actor is not dead
		{
			//action.Action->Perform(action.Actor, action.Location);
			return;
		}
	}

	bPerformingActions = false;
}

UCombatManager::UCombatManager()
{
	Instance = this;
}

//class UWorld* UCombatManager::GetWorld() const
//{
//	return (
//		!HasAnyFlags(RF_ClassDefaultObject) && !GetOuter()->HasAnyFlags(RF_BeginDestroyed) && !GetOuter()->IsUnreachable() 
//		? CastChecked<UGameInstance>(GetOuter())->GetWorld() : NULL
//		);
//}

void UCombatManager::StartCombat()
{
	/*if (!Instance->GetWorld())
	{
		UE_LOG(LogTemp, Error, TEXT("Combat Manager: GetWorld failed"));
		if (Instance->GetOuter())
		{
			UE_LOG(LogTemp, Error, TEXT("Combat Manager: %s"), *Instance->GetOuter()->GetName());

		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Combat Manager: No Outer"));
		}
		return;
	}*/

	Instance->bPreCombat = false;
	ACameraPawn::Instance->RevealMap();

	TArray<AActor*> temp;
	UE_LOG(LogTemp, Log, TEXT("Combat Manager: Sanity Check 1"));
	//UGameplayStatics::GetAllActorsOfClass(Instance->GetWorld(), AGTAIController::StaticClass(), temp);
	UGameplayStatics::GetAllActorsOfClass(GEngine->GameViewport->GetWorld(), AGTCharacter::StaticClass(), temp);
	if (temp.Num() == 0)
	{
		UE_LOG(LogTemp, Error, TEXT("0 GTCharacters found"));
		return;
	}
	//UE_LOG(LogTemp, Log, TEXT("temp.Num() = %d"), temp.Num());
	//UE_LOG(LogTemp, Log, TEXT("iQueue.Num() = %d"), Instance->InitiativeQueue.Num());
	Instance->InitiativeQueue.Empty(temp.Num());
	for (int i = 0; i < temp.Num(); i++)
	{
		bool bInserted = false;
		AGTCharacter* gtc = Cast<AGTCharacter>(temp[i]);
		for (int j = 0; j < Instance->InitiativeQueue.Num(); j++)
		{
			if (gtc->Initiative > Instance->InitiativeQueue[j]->Initiative)
			{
				Instance->InitiativeQueue.Insert(gtc, j);
				bInserted = true;
				break;
			}
		}
		if (!bInserted)
			Instance->InitiativeQueue.Add(gtc);

		if (gtc->ActorHasTag(FName("Party")))
			Instance->PartyCharacters.Add(gtc);
		else
			Instance->EnemyCharacters.Add(gtc); //TODO: ally characters would need separate array
	}

	Instance->InitiativeQueue[0]->BeginTurn();
	Instance->InitiativeIndex = 0;
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

	Instance->InitiativeQueue[Instance->InitiativeIndex]->BeginTurn();
}

class AGTCharacter* UCombatManager::ActingCharacter()
{
	return Instance->InitiativeQueue[Instance->InitiativeIndex];
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
	act.Actor = attacker;
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
	act.Actor = attacker;
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

void UCombatManager::CompleteAction()
{
	//if (Instance->bInterrupting)
	//{
	//	//TODO: check if attacker is still alive

	//	if (Instance->InterruptQueue.Num() > 0)
	//	{
	//		//TODO: check if attack is still valid target for interrupt

	//		FActionData interrupt = Instance->InterruptQueue[0];
	//		Instance->InterruptQueue.RemoveAt(0);

	//		interrupt.Action->Perform(interrupt.Actor, interrupt.RelativeLocation);
	//		UTacticsHUDCode::Instance->ShowAttackText(NSLOCTEXT("Combat", "UIInterrupt", "Interrupt!"));
	//	}
	//	else
	//	{
	//		Instance->bInterrupting = false;
	//		ActingController()->GetTacticsCharacter()->GetSprite()->SetPlayRate(1);
	//	}

	//	return;
	//}

	/*if (Instance->ReactionQueue.Num() > 0)
	{
		UE_LOG(LogTemp, Log, TEXT("React!"));
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("React!"));

		FActionData reaction = Instance->ReactionQueue[0];
		Instance->ReactionQueue.RemoveAt(0);

		reaction.Action->Perform(reaction.Actor, reaction.RelativeLocation);

		UTacticsHUDCode::Instance->ShowAttackText(NSLOCTEXT("Combat", "UIReaction", "Reaction!"));

		return;
	}*/

	//TODO: perform reactions

	Instance->bPerformingActions = false;
	if (ActingCharacter()->CurrentAP < 1) //TODO: replace 1 with minAP?
		ActingCharacter()->EndTurn();
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

bool UCombatManager::RollAttack(class UActionAttack* action, class AGTCharacter* attacker, TScriptInterface<ITargetableInterface> target)
{
	if (Instance)
	{
		//TODO: more in-depth calculations (eg acc bonus based on target, def bonus based on attacker, etc)
		return Instance->AttackRoll(attacker->GetAccuracy(action->AttackType), target->GetDefense(action->AttackType));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("No CombatManager"));
		return true;
	}
}
