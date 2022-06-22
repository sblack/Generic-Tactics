// Fill out your copyright notice in the Description page of Project Settings.


#include "TargetableInterface.h"
#include "../UI/GTHUDCode.h"

// Add default functionality here for any ITargetableInterface functions that are not pure virtual.

TScriptInterface<ITargetableInterface> ITargetableInterface::Targetable(AActor* actor)
{
	auto t = Cast<ITargetableInterface>(actor);
	if (t != nullptr) {
		TScriptInterface<ITargetableInterface> s = TScriptInterface<ITargetableInterface>();
		s.SetInterface(t);
		s.SetObject(actor);
		return s;
	}

	return TScriptInterface<ITargetableInterface>();
}

void ITargetableInterface::OnHit(bool bMajor)
{
	UGTHUDCode::Instance->FlashRed();
}