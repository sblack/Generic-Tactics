// Fill out your copyright notice in the Description page of Project Settings.


#include "ActionMove.h"
#include "../Character/GTCharacter.h"
#include "../Character/GTAIController.h"

void UActionMove::Perform(class AGTCharacter* user, FVector direction)
{
	Super::Perform(user, direction);

	//TArray<FVector> NavPath;
	//if (user->GetPathBack(Destination, NavPath))
	//{
	//	user->StartMoving(NavPath);
	//}
	//else
	//{
	//	//ActionMove should only be created if user can actually reach destination
	//	UE_LOG(LogTemp, Error, TEXT("ActionMove could not find path to destination"));
	//}
}

//void UActionMove::Resolve(class AGTCharacter* user, FVector direction)
//{
//	throw std::logic_error("The method or operation is not implemented.");
//}
