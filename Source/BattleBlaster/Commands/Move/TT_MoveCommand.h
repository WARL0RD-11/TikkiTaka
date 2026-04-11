// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "BattleBlaster/Commands/Command.h"
#include "CoreMinimal.h"

/**
 * 
 */
class BATTLEBLASTER_API TT_MoveCommand : public ICommand
{
public:
	TT_MoveCommand();

	void Execute() override;
};
