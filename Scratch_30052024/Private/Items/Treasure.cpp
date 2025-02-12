// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Treasure.h"
#include "Characters/ScratchCharacter.h"

void ATreasure::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    AScratchCharacter* ScratchCharacter = Cast<AScratchCharacter>(OtherActor);
    if (ScratchCharacter)
    {
        SpawnPickupSound();
        Destroy();
    }
}
