// Fill out your copyright notice in the Description page of Project Settings.


#include "GTHUDCode.h"
#include "Components/TextBlock.h"
#include "Components/RichTextBlock.h"

UGTHUDCode* UGTHUDCode::Instance;

void UGTHUDCode::UpdateDebugText(const FString& text, int32 line /*= 0*/)
{
	if (line < 0 || line >= 5) line = 0;

	DebugMessages[line] = text;


	FString temp = DebugMessages[0];
	for (int i = 1; i < 5; i++)
	{
		temp = temp + "\n" + DebugMessages[i];
	}

	DebugLog->SetText(FText::FromString(temp));
}

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

