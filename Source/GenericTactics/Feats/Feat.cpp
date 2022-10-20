// Fill out your copyright notice in the Description page of Project Settings.


#include "Feat.h"

FText UFeat::GetReqText()
{
	FText text = NSLOCTEXT("Feats", "Req", "Req: ");
	bool bFirstReq = true;
	if(MinLevel > 1)
	{
		text = FText::Format(NSLOCTEXT("Feats", "AddLvl", "{0}Lvl {1}"), text, FText::AsNumber(MinLevel));
		bFirstReq = false;
	}
	for (UFeat* preReq : PreReqs)
	{
		if (bFirstReq)
		{
			text = FText::Format(NSLOCTEXT("Feats", "AddFirstPreReq", "{0}{1}"), text, preReq->Name);
			bFirstReq = false;
		}
		else
		{
			text = FText::Format(NSLOCTEXT("Feats", "AddPreReq", "{0}, {1}"), text, preReq->Name);
		}
	}

	return text;
}
