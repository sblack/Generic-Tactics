// Fill out your copyright notice in the Description page of Project Settings.


#include "BlockingLayerCode.h"
#include "Components/TextBlock.h"
#include "Components/Border.h"
#include "Components/WidgetSwitcher.h"

UBlockingLayerCode* UBlockingLayerCode::Instance;

void UBlockingLayerCode::NativeConstruct()
{
	Super::NativeConstruct();

	Instance = this;
}

void UBlockingLayerCode::NativeDestruct()
{
	if (Instance == this)
		Instance = nullptr;

	Super::NativeDestruct();
}

void UBlockingLayerCode::StopBlocking()
{
	BaseBorder->SetVisibility(ESlateVisibility::Hidden);
}

void UBlockingLayerCode::BlockWithText(FText text)
{
	BaseBorder->SetVisibility(ESlateVisibility::Visible);
	Switcher->SetActiveWidget(TextBlock);
	TextBlock->SetText(text);
}
