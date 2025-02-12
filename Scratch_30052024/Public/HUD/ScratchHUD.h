// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "ScratchHUD.generated.h"

/**
 * 
 */
UCLASS()
class SCRATCH_30052024_API AScratchHUD : public AHUD
{
	GENERATED_BODY()
    
protected:
    virtual void BeginPlay() override;
	
private:
    UPROPERTY(EditDefaultsOnly, Category = Scratch)
    TSubclassOf<class UScratchOverlay> ScratchOverlayClass;
    
    UPROPERTY()
    UScratchOverlay* ScratchOverlay;
    
public:
    FORCEINLINE UScratchOverlay* GetScratchOverlay() const {return ScratchOverlay;}
};
