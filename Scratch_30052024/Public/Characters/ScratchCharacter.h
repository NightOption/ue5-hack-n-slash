// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseCharacter.h"
#include "CharacterTypes.h"
#include "ScratchCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class AItem;
class UAnimMontage;

UCLASS()
class SCRATCH_30052024_API AScratchCharacter : public ABaseCharacter
{
	GENERATED_BODY()

public:
	AScratchCharacter();
    virtual void Tick(float DeltaTime) override;
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
    
    
    
protected:
    virtual void BeginPlay() override;
    
    /**
     Callbacks for input
     */
    void MoveForward(float Value);
    void MoveRight(float Value);
    void Turn(float Value);
    void LookUp(float Value);
    void EKeyPressed();
    virtual void Attack() override;
    
    /**
     Play montage functions
     */
    virtual void PlayAttackMontage() override;
    
    virtual void AttackEnd() override;
    virtual bool CanAttack() override;
    
    void PlayEquipMontage(const FName& SectionName);
    bool CanDisarm();
    bool CanArm();
    
    UFUNCTION(BlueprintCallable)
    void Disarm();
    
    UFUNCTION(BlueprintCallable)
    void Arm();
    
    UFUNCTION(BlueprintCallable)
    void FinishEquipping();

private:
    
    ECharacterState CharacterState = ECharacterState::ECS_Unequipped;
    
    UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
    EActionState ActionState = EActionState::EAS_Unoccupied;
    
    UPROPERTY(VisibleAnywhere)
    USpringArmComponent* CameraBoom;
    
    UPROPERTY(VisibleAnywhere)
    UCameraComponent* ViewCamera;

    UPROPERTY(VisibleInstanceOnly)
    AItem* OverlappingItem;
    
    UPROPERTY(EditDefaultsOnly, Category = Montages)
    UAnimMontage* EquipMontage;
    
public:
    FORCEINLINE void SetOverlappingItem(AItem* Item) { OverlappingItem = Item; }
    FORCEINLINE ECharacterState GetCharacterState () const { return CharacterState; }
};
