// Fill out your copyright notice in the Description page of Project Settings.


#include "TT_UI_ScreenMsg.h"
#include "Components/TextBlock.h"	

void UTT_UI_ScreenMsg::SetScreenMsgText(const FText& NewText)
{
	if (ScreenMsg)
	{
		ScreenMsg->SetText(NewText);
	}
}
