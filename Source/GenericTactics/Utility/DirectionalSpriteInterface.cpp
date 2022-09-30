// Fill out your copyright notice in the Description page of Project Settings.


#include "DirectionalSpriteInterface.h"

TArray<TScriptInterface<IDirectionalSpriteInterface>> IDirectionalSpriteInterface::Sprites;

// Add default functionality here for any IDirectionalSpriteInterface functions that are not pure virtual.

TScriptInterface<IDirectionalSpriteInterface> IDirectionalSpriteInterface::DirectionalSprite(AActor* actor)
{
	auto t = Cast<IDirectionalSpriteInterface>(actor);
	if (t != nullptr) {
		TScriptInterface<IDirectionalSpriteInterface> s = TScriptInterface<IDirectionalSpriteInterface>();
		s.SetInterface(t);
		s.SetObject(actor);
		return s;
	}

	return TScriptInterface<IDirectionalSpriteInterface>();
}
