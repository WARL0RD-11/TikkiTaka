// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TT_UI_ScreenMsg.generated.h"

/**
 * 
 */
class UTextBlock;

UCLASS()
class BATTLEBLASTER_API UTT_UI_ScreenMsg : public UUserWidget
{
	GENERATED_BODY()

public: 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "ScreenMsg")
	TObjectPtr<UTextBlock> ScreenMsg;
	
	UFUNCTION(BlueprintCallable, Category = "ScreenMsg")
	void SetScreenMsgText(const FText& NewText);

};
