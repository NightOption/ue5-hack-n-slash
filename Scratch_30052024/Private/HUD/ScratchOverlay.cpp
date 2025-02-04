// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/ScratchOverlay.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"

void UScratchOverlay::SetHealthBarPercent(float Percent)
{
    if (HealthProgressBar)
    {
        HealthProgressBar->SetPercent(Percent);
    }
}

void UScratchOverlay::SetStaminaBarPercent(float Percent)
{
    if (StaminaProgressBar)
    {
        StaminaProgressBar->SetPercent(Percent);
    }
}

void UScratchOverlay::SetGold(int32 Gold)
{
    if (GoldText)
    {
        GoldText->SetText(FText::FromString(FString::Printf(TEXT("%d"), Gold)));
    }
}

void UScratchOverlay::SetSouls(int32 Souls)
{
    if (SoulsText)
    {
        SoulsText->SetText(FText::FromString(FString::Printf(TEXT("%d"), Souls)));
    }
}
