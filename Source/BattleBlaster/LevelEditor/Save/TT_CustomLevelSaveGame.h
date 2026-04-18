// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "../Data/TT_CustomLevelTypes.h"
#include "TT_CustomLevelSaveGame.generated.h"

/**
 * 
 */
UCLASS()
class BATTLEBLASTER_API UTT_CustomLevelSaveGame : public USaveGame
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Custom Level")
	FTT_CustomLevelDefinition SavedLevel;	
	
};
