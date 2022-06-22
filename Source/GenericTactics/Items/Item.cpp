// Fill out your copyright notice in the Description page of Project Settings.


#include "Item.h"

FText UItem::GetItemText()
{
	return FText::Format(NSLOCTEXT("Items", "Description", "<Bold>Description:</> {0}\n"), Description);
}
