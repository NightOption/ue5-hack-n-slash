// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/ScratchAnimInstance.h"
#include "Characters/ScratchCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

void UScratchAnimInstance::NativeInitializeAnimation()
{
    Super::NativeInitializeAnimation();
    
    ScratchCharacter = Cast<AScratchCharacter>(TryGetPawnOwner());
    if (ScratchCharacter)
    {
       ScratchCharacterMovement = ScratchCharacter->GetCharacterMovement();
    }
}

void UScratchAnimInstance::NativeUpdateAnimation(float DeltaTime)
{
    Super::NativeUpdateAnimation(DeltaTime);
    
    if (ScratchCharacterMovement)
    {
        GroundSpeed = UKismetMathLibrary::VSizeXY(ScratchCharacterMovement->Velocity);
        IsFalling = ScratchCharacterMovement->IsFalling();
        CharacterState = ScratchCharacter->GetCharacterState();
        ActionState = ScratchCharacter->GetActionState();
        DeathPose = ScratchCharacter->GetDeathPose();
    }
}
