// Fill out your copyright notice in the Description page of Project Settings.


#include "ActionDirect.h"
#include "AreaOfEffect.h"

TArray<FVector> UActionDirect::GetAffectedArea(IActionSource source, FVector target)
{
	if (Area)
		return Area->GetTargetedSquares(source, target);
	else
	{
		TArray<FVector> result;
		result.Add(target);
		return result;
	}
}
