// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/ScratchHUD.h"
#include "HUD/ScratchOverlay.h"
void AScratchHUD::BeginPlay()
{
    Super::BeginPlay();
    
    UWorld* World = GetWorld();
    if (World)
    {
        APlayerController* Controller = World->GetFirstPlayerController();
        if (Controller && ScratchOverlayClass)
        {
            ScratchOverlay = CreateWidget<UScratchOverlay>(Controller, ScratchOverlayClass);
            ScratchOverlay->AddToViewport();
        }
    }
}
