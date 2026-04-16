// Fill out your copyright notice in the Description page of Project Settings.


#include "TikkiTakaGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "BattleBlaster/Pawn/Tank/TT_TankPawn.h"
#include "BattleBlaster/Pawn/Tower/TT_TowerPawn.h"
#include "BattleBlaster/Controllers/TT_PlayerController.h"
#include "BattleBlaster/GI/TT_GameInstance.h"
#include "BattleBlaster/UI/TT_UI_ScreenMsg.h"



void ATikkiTakaGameMode::BeginPlay()
{
	Super::BeginPlay();
	PlayerTank = Cast<ATT_TankPawn>(UGameplayStatics::GetPlayerPawn(this, 0));
	PC = Cast<ATT_PlayerController>(UGameplayStatics::GetPlayerController(this, 0));

	Countdown = LevelStartDelay;
	GetWorldTimerManager().SetTimer(CountdownTimerHandle, this, &ATikkiTakaGameMode::CountdownTimerTick, 1.0f, true);	


	if (ScreenMsgWidgetClass)
	{
		ScreenMsgWidget = CreateWidget<UTT_UI_ScreenMsg>(PC, ScreenMsgWidgetClass);
		ScreenMsgWidget->AddToViewport();
	}
}

void ATikkiTakaGameMode::CountdownTimerTick()
{
	if (Countdown <= 0)
	{
		GetWorldTimerManager().ClearTimer(CountdownTimerHandle);
		PC->SetPlayerInputDisabled(false);
		if (ScreenMsgWidget)
		{
			ScreenMsgWidget->SetVisibility(ESlateVisibility::Hidden);
		}
	}
	else
	{
		Countdown--;

		if (ScreenMsgWidget)
		{
			ScreenMsgWidget->SetScreenMsgText(FText::FromString(FString::Printf(TEXT("Game starts in: %d"), Countdown)));
		}
	}
}

void ATikkiTakaGameMode::ActorDied(AActor* DeadActor)
{
	bool bIsGameOver = false;	

	if (!IsValid(DeadActor))
	{
		return;
	}

	if (DeadActor == PlayerTank)
	{
		PlayerTank->HandleDestruction();

		if (PC)
		{
			PlayerTank->DisableInput(PC);
			PC->SetPlayerInputDisabled(true);
			PC->SetShowMouseCursor(false);
		}
		bIsGameOver = true;
		bIsVictory = false;	
	}

	if (ATT_TowerPawn* Tower = Cast<ATT_TowerPawn>(DeadActor))
	{
		Tower->HandleDestruction();

		DeadActor->Destroy();

		const int32 RemainingTowers = GetActiveTowerCount();

		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(
				-1,
				5.f,
				FColor::Green,
				FString::Printf(TEXT("Tower Destroyed. Remaining Towers: %d"), RemainingTowers)
			);
		}

		if (RemainingTowers == 0)
		{
			bIsGameOver = true;
			bIsVictory = true;
		}

	}

	if (bIsGameOver)
	{
		ScreenMsgWidget->SetVisibility(ESlateVisibility::Visible);
		GetWorldTimerManager().SetTimer(RestartTimerHandle, this, &ATikkiTakaGameMode::GameLevelTransition, RestartDelay, false);
		bIsVictory ? ScreenMsgWidget->SetScreenMsgText(FText::FromString(TEXT("You Win!"))) : 
			ScreenMsgWidget->SetScreenMsgText(FText::FromString(TEXT("You Died!")));
	}

}

void ATikkiTakaGameMode::RegisterTower(ATT_TowerPawn* Tower)
{
	if (!IsValid(Tower)) return;

	ActiveTowers.Add(Tower);
}

void ATikkiTakaGameMode::UnregisterTower(ATT_TowerPawn* Tower)
{
	if (!IsValid(Tower)) return;

	ActiveTowers.Remove(Tower);
}

int32 ATikkiTakaGameMode::GetActiveTowerCount() const
{
	return ActiveTowers.Num();
}

void ATikkiTakaGameMode::GameLevelTransition()
{
	UTT_GameInstance* GI = Cast<UTT_GameInstance>(GetGameInstance());
	if(!GI) return;	

	if(bIsVictory)
	{
		GI->LoadNextLevel();	
	}
	else
	{
		GI->RestartCurrentLevel();

	}

}	