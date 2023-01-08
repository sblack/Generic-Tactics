// Fill out your copyright notice in the Description page of Project Settings.


#include "CommandMenuCode.h"
#include "../Player/GTPlayerController.h"
#include "../Character/GTCharacter.h"
#include "Components/Button.h"
#include "Widgets/Input/SButton.h"

void UCommandMenuCode::DeselectMain()
{
	if (!SelectedMain) return;

	Sub->SetVisibility(ESlateVisibility::Collapsed);
	AGTPlayerController::Instance->CancelTarget();

	//SelectedMain->SetBackgroundColor(FLinearColor::White);
	//SelectedMain->BackgroundColor = FLinearColor::White;
	SelectedMain = nullptr;

	//somehow, a nullptr isn't enough. What the fuck
	if (SelectedSub != nullptr)
	{
		//SelectedSub->SetBackgroundColor(FLinearColor::White);
		SelectedSub = nullptr;
	}

	LabelTextUnhovered = FText::GetEmpty();
}

void UCommandMenuCode::UpdateSubMenu()
{
	for (int i = 0; i < 7; i++)
	{
		int ind = i + SubMenuOffset;
		if (ind >= SubActions.Num() || !SubActions[ind])
		{
			//want next button to stay in position, so hide instead of collapsing un-needed buttons
			SubButtons[i]->SetVisibility(bNextButton ? ESlateVisibility::Hidden : ESlateVisibility::Collapsed);
		}
		else
		{
			SubButtons[i]->SetVisibility(ESlateVisibility::Visible);
			if (SubActions[ind]->Icon)
			{
				SubIcons[i]->SetBrushFromTexture(SubActions[ind]->Icon);
			}
		}
	}

	if (bNextButton)
	{
		SubButtons[7]->SetVisibility(ESlateVisibility::Visible);
		SubIcons[7]->SetBrushFromTexture(NextIcon);
	}
	else
	{
		//if no next button, offset is going to be 0
		if (7 >= SubActions.Num() || !SubActions[7])
		{
			SubButtons[7]->SetVisibility(ESlateVisibility::Collapsed);
		}
		else
		{
			SubButtons[7]->SetVisibility(ESlateVisibility::Visible);
			if (SubActions[7]->Icon)
			{
				SubIcons[7]->SetBrushFromTexture(SubActions[7]->Icon);
			}
		}
	}
}

void UCommandMenuCode::ButtonUnhovered()
{
	if (!LabelText)
	{
		UE_LOG(LogTemp,Error,TEXT("LabelText is null, somehow"));
	}
	else
		LabelText->SetText(LabelTextUnhovered);


	if(LabelTextUnhovered.IsEmpty())
	{
		if (!Label)
		{
			UE_LOG(LogTemp, Error, TEXT("Label is null, somehow"));
		}
		else
			Label->SetVisibility(ESlateVisibility::Hidden);
	}
}

void UCommandMenuCode::MoveClicked()
{
	bool bWasMove = (SelectedMain == MoveButton);
	DeselectMainBP();
	if (!bWasMove)
	{
		SelectedMain = MoveButton;
		MoveButton->SetBackgroundColor(FLinearColor::Yellow);
		AGTPlayerController::Instance->BeginTargetMove();
		LabelTextUnhovered = NSLOCTEXT("Command", "Move", "Move");
		//TODO: include remaining move
	}
}

void UCommandMenuCode::AttackClicked()
{
	bool bWasAttack = (SelectedMain == AttackButton);
	DeselectMainBP();
	if (!bWasAttack)
	{
		SelectedMain = AttackButton;
		AttackButton->SetBackgroundColor(FLinearColor::Yellow);

		Sub->SetVisibility(ESlateVisibility::Visible);

		SubMenuOffset = 0;
		SubActions = AGTPlayerController::Instance->GetActiveCharacter()->GetAllAttacks();
		bNextButton = SubActions.Num() > 8;
		UpdateSubMenu();
		LabelTextUnhovered = NSLOCTEXT("Command", "Attack", "Attack");
	}
}

void UCommandMenuCode::BindButtons()
{
	MoveButton->OnClicked.AddDynamic(this, &UCommandMenuCode::MoveClicked);
	SButton* ButtonWidget = (SButton*)&(MoveButton->TakeWidget().Get());
	ButtonWidget->SetOnHovered(FSimpleDelegate::CreateLambda([this]()
		{
			Label->SetVisibility(ESlateVisibility::Visible);
			LabelText->SetText(NSLOCTEXT("Command", "Move", "Move"));
		}));
	MoveButton->OnUnhovered.AddDynamic(this, &UCommandMenuCode::ButtonUnhovered);

	AttackButton->OnClicked.AddDynamic(this, &UCommandMenuCode::AttackClicked);
	ButtonWidget = (SButton*)&(AttackButton->TakeWidget().Get());
	ButtonWidget->SetOnHovered(FSimpleDelegate::CreateLambda([this]()
		{
			Label->SetVisibility(ESlateVisibility::Visible);
			LabelText->SetText(NSLOCTEXT("Command", "Attack", "Attack"));
		}));
	AttackButton->OnUnhovered.AddDynamic(this, &UCommandMenuCode::ButtonUnhovered);

	//TechButton->OnClicked.AddDynamic(this, &UCommandMenuCode::AttackClicked);
	ButtonWidget = (SButton*)&(TechButton->TakeWidget().Get());
	ButtonWidget->SetOnHovered(FSimpleDelegate::CreateLambda([this]()
		{
			Label->SetVisibility(ESlateVisibility::Visible);
			LabelText->SetText(NSLOCTEXT("Command", "Tech", "Techniques"));
		}));
	TechButton->OnUnhovered.AddDynamic(this, &UCommandMenuCode::ButtonUnhovered);

	//MagicButton->OnClicked.AddDynamic(this, &UCommandMenuCode::AttackClicked);
	ButtonWidget = (SButton*)&(MagicButton->TakeWidget().Get());
	ButtonWidget->SetOnHovered(FSimpleDelegate::CreateLambda([this]()
		{
			Label->SetVisibility(ESlateVisibility::Visible);
			LabelText->SetText(NSLOCTEXT("Command", "Magic", "Magic"));
		}));
	MagicButton->OnUnhovered.AddDynamic(this, &UCommandMenuCode::ButtonUnhovered);

	//ModalButton->OnClicked.AddDynamic(this, &UCommandMenuCode::AttackClicked);
	ButtonWidget = (SButton*)&(ModalButton->TakeWidget().Get());
	ButtonWidget->SetOnHovered(FSimpleDelegate::CreateLambda([this]()
		{
			Label->SetVisibility(ESlateVisibility::Visible);
			LabelText->SetText(NSLOCTEXT("Command", "Modal", "Modal"));
		}));
	ModalButton->OnUnhovered.AddDynamic(this, &UCommandMenuCode::ButtonUnhovered);

	//ItemsButton->OnClicked.AddDynamic(this, &UCommandMenuCode::AttackClicked);
	ButtonWidget = (SButton*)&(ItemsButton->TakeWidget().Get());
	ButtonWidget->SetOnHovered(FSimpleDelegate::CreateLambda([this]()
		{
			Label->SetVisibility(ESlateVisibility::Visible);
			LabelText->SetText(NSLOCTEXT("Command", "Items", "Inventory"));
		}));
	ItemsButton->OnUnhovered.AddDynamic(this, &UCommandMenuCode::ButtonUnhovered);

	//InfoButton->OnClicked.AddDynamic(this, &UCommandMenuCode::AttackClicked);
	ButtonWidget = (SButton*)&(InfoButton->TakeWidget().Get());
	ButtonWidget->SetOnHovered(FSimpleDelegate::CreateLambda([this]()
		{
			Label->SetVisibility(ESlateVisibility::Visible);
			LabelText->SetText(NSLOCTEXT("Command", "Info", "Info"));
		}));
	InfoButton->OnUnhovered.AddDynamic(this, &UCommandMenuCode::ButtonUnhovered);

	//EndButton->OnClicked.AddDynamic(this, &UCommandMenuCode::AttackClicked);
	ButtonWidget = (SButton*)&(EndButton->TakeWidget().Get());
	ButtonWidget->SetOnHovered(FSimpleDelegate::CreateLambda([this]()
		{
			Label->SetVisibility(ESlateVisibility::Visible);
			LabelText->SetText(NSLOCTEXT("Command", "End", "End Turn"));
		}));
	EndButton->OnUnhovered.AddDynamic(this, &UCommandMenuCode::ButtonUnhovered);


	for (int i = 0; i < 8; i++)
	{
		ButtonWidget = (SButton*)&(SubButtons[i]->TakeWidget().Get());
		ButtonWidget->SetOnClicked(FOnClicked::CreateLambda([this, i]()
			{
				SubClicked(i);
				return FReply::Handled();
			}));

		ButtonWidget->SetOnHovered(FSimpleDelegate::CreateLambda([this, i]()
			{
				SubHovered(i);
			}));

		SubButtons[i]->OnUnhovered.AddDynamic(this, &UCommandMenuCode::ButtonUnhovered);
	}
}

void UCommandMenuCode::SubClicked(int32 index)
{
	if (bNextButton && index == 7)
	{
		SubMenuOffset += 7;
		if (SubMenuOffset >= SubActions.Num()) SubMenuOffset = 0;
		return;
	}

	bool bCancel = (SelectedSub == SubButtons[index]);
	if (SelectedSub)
	{
		SelectedSub->SetBackgroundColor(FLinearColor::White);
		AGTPlayerController::Instance->CancelTarget();
		SelectedSub = nullptr;
	}
	if (!bCancel)
	{
		SelectedSub = SubButtons[index];
		SelectedSub->SetBackgroundColor(FLinearColor::Yellow);
		AGTPlayerController::Instance->BeginTargetAction(SubActions[index + SubMenuOffset]);
		LabelTextUnhovered = SubActions[index + SubMenuOffset]->Name;
	}
}

void UCommandMenuCode::SubHovered(int32 index)
{
	Label->SetVisibility(ESlateVisibility::Visible);
	if (bNextButton && index == 7)
	{
		LabelText->SetText(NSLOCTEXT("Command", "Next", "Next Page"));
	}
	else
	{
		LabelText->SetText(SubActions[index + SubMenuOffset]->Name);
	}
}

void UCommandMenuCode::Reset()
{
	DeselectMainBP();
	ButtonUnhovered();
}
