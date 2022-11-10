// Fill out your copyright notice in the Description page of Project Settings.


#include "MonsterBase.h"
#include "TimerManager.h"
#include "StatsBlock.h"

void AMonsterBase::InitMaterials()
{
	AlterColors();
	BodyDMI = GetSprite()->CreateDynamicMaterialInstance(0);
	BodyDMI->SetVectorParameterValue(TEXT("Color0"), Color0);
	BodyDMI->SetVectorParameterValue(TEXT("Color1"), Color1);
	BodyDMI->SetVectorParameterValue(TEXT("Color2"), Color2);
}

void AMonsterBase::SetStats()
{
	for (EVitals vit : TEnumRange<EVitals>())
	{
		int32* val = MaxVitals.Find(vit);
		if(val != nullptr)
			Stats->MaxVitals[vit] = *val;
		else
			Stats->MaxVitals[vit] = 0;
	}
	if (Stats->MaxVitals[EVitals::Health] == 0)
	{
		UE_LOG(LogTemp, Error, TEXT("%s has 0 HP by default. Setting to 10 temporarily."), *GetName());
		Stats->MaxVitals[EVitals::Health] = 10;
	}
	Stats->CurrentVitals[EVitals::Health] = Stats->MaxVitals[EVitals::Health];
	Stats->CurrentVitals[EVitals::Stamina] = Stats->MaxVitals[EVitals::Stamina];
	//mana starts combat at 0, unlike health and stamina

	for (EDefenseType def : TEnumRange<EDefenseType>())
	{
		int32* val = Defense.Find(def);
		if (val != nullptr)
			Stats->Defense[def] = *val;
		else
			Stats->Defense[def] = 0;
	}

	for (EAttackType att : TEnumRange<EAttackType>())
	{
		int32* val = Accuracy.Find(att);
		if (val != nullptr)
			Stats->Accuracy[att] = *val;
		else
			Stats->Accuracy[att] = 0;
	}

	for (EDamageType dam : TEnumRange<EDamageType>())
	{
		int32* val = Resist.Find(dam);
		if (val != nullptr)
			Stats->Resist[dam] = *val;
		else
			Stats->Resist[dam] = 0;
	}

	Stats->MaxActions = MaxActions;
	Stats->MoveSpeed = MoveSpeed;
	Stats->DetectionRadius = DetectionRadius;
}

void AMonsterBase::AdvanceAI()
{
	if (!IsPartyCharacter()) //TODO: allow for AI control on Party Characters?
	{
		bool bEndTurn = true; //if AI can't perform any objectives, end turn
		for (int i = 0; i < AIObjectives.Num(); i++)
		{
			if (AIObjectives[i]->Attempt(this))
			{
				bEndTurn = false;
				UE_LOG(LogTemp, Log, TEXT("%s successful"), *AIObjectives[i]->GetDebugString());
				break;
			}
			else
			{
				UE_LOG(LogTemp, Log, TEXT("%s failed"), *AIObjectives[i]->GetDebugString());
			}
		}

		if (bEndTurn)
		{
			UE_LOG(LogTemp, Log, TEXT("all objectives impossible, ending turn"));

			FTimerHandle TimerHandle;
			GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &AGTCharacter::EndTurn, 1, false);
		}
	}
}
