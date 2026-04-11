// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
class ICommand
{
public:
	virtual ~ICommand() {};
	virtual void Execute() = 0;
};