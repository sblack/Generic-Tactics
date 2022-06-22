// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ItemEquipment.h"
#include "ItemWeapon.generated.h"

/**
 * 
 */
UCLASS()
class GENERICTACTICS_API UItemWeapon : public UItemEquipment
{
	GENERATED_BODY()
public:

	// Note the CanEditChange() function is only available when compiling with the editor.
// Make sure to wrap it with the WITH_EDITOR define or your builds fail!
#if WITH_EDITOR
	virtual bool CanEditChange(const FProperty* InProperty) const override;
#endif

	virtual void PostInitProperties() override;
};
