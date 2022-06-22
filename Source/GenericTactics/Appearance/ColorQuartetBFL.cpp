// Fill out your copyright notice in the Description page of Project Settings.


#include "ColorQuartetBFL.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "../Utility/GTBFL.h"

FLinearColor UColorQuartetBFL::GetQuartetMember(struct FColorQuartet Target, uint8 index)
{
	switch (index)
	{
	case 0: return Target.Color0;
	case 1: return Target.Color1;
	case 2: return Target.Color2;
	case 3: return Target.Color3;
		default:
			UE_LOG(LogTemp, Error, TEXT("invalid index for quartet (%d)"), index);
			return FLinearColor::Black;
	}
}

FLinearColor UColorQuartetBFL::SetQuartetMember(struct FColorQuartet& Target, uint8 index, FLinearColor Color)
{
	switch (index)
	{
	case 0: Target.Color0 = Color; return Target.Color0;
	case 1: Target.Color1 = Color; return Target.Color1;
	case 2: Target.Color2 = Color; return Target.Color2;
	case 3: Target.Color3 = Color; return Target.Color3;
	default:
		UE_LOG(LogTemp, Error, TEXT("invalid index for quartet (%d)"), index);
		return FLinearColor::Black;
	}
}

void UColorQuartetBFL::SetColorQuartetParameter(class UMaterialInstanceDynamic* Target, struct FColorQuartet Colors)
{
	Target->SetVectorParameterValue(FName(TEXT("Color0")), Colors.Color0);
	Target->SetVectorParameterValue(FName(TEXT("Color1")), Colors.Color1);
	Target->SetVectorParameterValue(FName(TEXT("Color2")), Colors.Color2);
	Target->SetVectorParameterValue(FName(TEXT("Color3")), Colors.Color3);
}

FColorQuartet UColorQuartetBFL::HSLtoRGBQuartet(FColorQuartet Target)
{
	FColorQuartet result;
	result.Color0 = UGTBFL::HSLtoRGB(Target.Color0);
	result.Color1 = UGTBFL::HSLtoRGB(Target.Color1);
	result.Color2 = UGTBFL::HSLtoRGB(Target.Color2);
	result.Color3 = UGTBFL::HSLtoRGB(Target.Color3);

	return result;
}

FColorQuartet FColorQuartet::ChangeTeamHue(float Hue, int TeamColors)
{
	FColorQuartet result;
	result.Color0 = Color0; if (TeamColors > 0) result.Color0.R = Hue;
	result.Color1 = Color1; if (TeamColors > 1) result.Color1.R = Hue;
	result.Color2 = Color2; if (TeamColors > 2) result.Color2.R = Hue;
	result.Color3 = Color3; if (TeamColors > 3) result.Color3.R = Hue;

	return result;
}
