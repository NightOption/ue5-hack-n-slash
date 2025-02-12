// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "CharacterTypes.h"
#include "ScratchAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class SCRATCH_30052024_API UScratchAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
public:
    virtual void NativeInitializeAnimation() override;
    virtual void NativeUpdateAnimation(float DeltaTime) override;
    
    UPROPERTY(BlueprintReadOnly)
    class AScratchCharacter* ScratchCharacter;
    
    UPROPERTY(BlueprintReadOnly, Category = Movement)
    class UCharacterMovementComponent* ScratchCharacterMovement;
    
    UPROPERTY(BlueprintReadOnly, Category = Movement)
    float GroundSpeed;
    
    UPROPERTY(BlueprintReadOnly, Category = Movement)
    bool IsFalling;
    
    UPROPERTY(BlueprintReadOnly, Category = Movement)
    ECharacterState CharacterState;
    
    UPROPERTY(BlueprintReadOnly, Category = Movement)
    EActionState ActionState;
    
    UPROPERTY(BlueprintReadOnly, Category = Movement)
    TEnumAsByte<EDeathPose> DeathPose;
};
