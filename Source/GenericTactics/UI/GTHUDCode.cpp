// Fill out your copyright notice in the Description page of Project Settings.


#include "GTHUDCode.h"
#include "Components/RichTextBlock.h"

UGTHUDCode* UGTHUDCode::Instance;

void UGTHUDCode::LogCombatMessage(FText text)
{
	if (!CombatLog)
	{
		UE_LOG(LogTemp, Error, TEXT("CombatLog not set"));
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
		CombatLog->SetText(text);
	}
	else
	{
		FString temp = Messages[TopMessage].ToString();
		for (int i = 1; i < MessageCount; i++)
		{
			temp = temp + "\n" + Messages[(TopMessage + i) % MaxMessages].ToString();
		}

		CombatLog->SetText(FText::FromString(temp));
	}
}

void UGTHUDCode::NativeConstruct()
{
	Super::NativeConstruct();

	Instance = this;
	Messages.Reserve(MaxMessages);
}

void UGTHUDCode::NativeDestruct()
{
	if (Instance == this)
		Instance = nullptr;

	Super::NativeDestruct();
}

