// Fill out your copyright notice in the Description page of Project Settings.


#include "ActionSourceInterface.h"
#include "../Combat/CombatManager.h"
#include "../UI/GTHUDCode.h"

// Add default functionality here for any IActionSourceInterface functions that are not pure virtual.

TScriptInterface<IActionSourceInterface> IActionSourceInterface::ActionSource(AActor* actor)
{
	auto t = Cast<IActionSourceInterface>(actor);
	if (t != nullptr) {
		TScriptInterface<IActionSourceInterface> s = TScriptInterface<IActionSourceInterface>();
		s.SetInterface(t);
		s.SetObject(actor);
		return s;
	}

	return TScriptInterface<IActionSourceInterface>();
}

void IActionSourceInterface::EndTurn()
{
	UCombatManager::AdvanceInitiative();
}
