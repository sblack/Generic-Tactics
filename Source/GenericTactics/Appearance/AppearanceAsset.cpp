// Fill out your copyright notice in the Description page of Project Settings.


#include "AppearanceAsset.h"

UAppearanceSetAsset::UAppearanceSetAsset()
{
	for (int i = 0; i < 10; i++)
	{
		DefaultColorsRGB.AddDefaulted();
		DefaultColorsRGB[i].Color0 = FLinearColor(.1f, .1f, .9f);
		DefaultColorsRGB[i].Color1 = FLinearColor(.3f, .3f, .9f);
		DefaultColorsRGB[i].Color2 = FLinearColor(.5f, .5f, .9f);
		DefaultColorsRGB[i].Color3 = FLinearColor(.1f, .1f, .1f);
		TeamColors.Add(1);
	}
}
