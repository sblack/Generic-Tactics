// Fill out your copyright notice in the Description page of Project Settings.


#include "CombatLogCode.h"
#include "Components/TextBlock.h"

UCombatLogCode* UCombatLogCode::Instance;

void UCombatLogCode::NativeConstruct()
{
	Super::NativeConstruct();

	Instance = this;
	Messages.Reserve(MaxMessages);
}

void UCombatLogCode::NativeDestruct()
{
	if (Instance == this)
		Instance = nullptr;

	Super::NativeDestruct();
}

void UCombatLogCode::LogMessage(FText text)
{
	if (!TextBlock)
	{
		UE_LOG(LogTemp, Error, TEXT("CombatLog TextBlock not set"));
		return;
	}
	UE_LOG(LogTemp, Log, TEXT("LogMessage: %s"), *text.ToString());

	if (MessageCount < MaxMessages)
	{
		Messages.Add(text);
		MessageCount++;
	}
	else
	{
		Messages[TopMessage] = text;
		TopMessage = (TopMessage + 1) % MaxMessages;
	}

	if (MessageCount == 1)
	{
		TextBlock->SetText(text);
	}
	else
	{
		FString temp = Messages[TopMessage].ToString();
		for (int i = 1; i < MessageCount; i++)
		{
			temp = temp + "\n" + Messages[(TopMessage + 1) % MaxMessages].ToString();
		}

		TextBlock->SetText(FText::FromString(temp));
	}
}