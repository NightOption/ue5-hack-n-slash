// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/ScratchCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Items/Item.h"
#include "Items/Weapons/Weapon.h"
#include "Animation/AnimMontage.h"
#include "Components/BoxComponent.h"

AScratchCharacter::AScratchCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
    
    bUseControllerRotationPitch = false;
    bUseControllerRotationYaw = false;
    bUseControllerRotationRoll = false;
    
    GetCharacterMovement()->bOrientRotationToMovement = true;
    GetCharacterMovement()->RotationRate = FRotator(0.f, 300.f, 0.f);

    CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
    CameraBoom->SetupAttachment(GetRootComponent());
    CameraBoom->TargetArmLength = 300.f;
    
    ViewCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("ViewCamera"));
    ViewCamera->SetupAttachment(CameraBoom);
}

void AScratchCharacter::BeginPlay()
{
	Super::BeginPlay();
	
    Tags.Add(FName("Scratch"));
}

void AScratchCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AScratchCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

    PlayerInputComponent->BindAxis(FName("MoveForward"), this, &AScratchCharacter::MoveForward);
    PlayerInputComponent->BindAxis(FName("MoveRight"), this, &AScratchCharacter::MoveRight);
    PlayerInputComponent->BindAxis(FName("Turn"), this, &AScratchCharacter::Turn);
    PlayerInputComponent->BindAxis(FName("LookUp"), this, &AScratchCharacter::LookUp);
    
    PlayerInputComponent->BindAction(FName("Jump"), IE_Pressed, this, &ACharacter::Jump);
    PlayerInputComponent->BindAction(FName("Equip"), IE_Pressed, this, &AScratchCharacter::EKeyPressed);
    PlayerInputComponent->BindAction(FName("Attack"), IE_Pressed, this, &AScratchCharacter::Attack);
}

void AScratchCharacter::MoveForward(float Value)
{
    if (ActionState != EActionState::EAS_Unoccupied) return;
    if (Controller && (Value != 0.f))
    {
        // find out which way is forward
        const FRotator ControlRotation = GetControlRotation();
        const FRotator YawRotation(0.f, ControlRotation.Yaw, 0.f);
        
        const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
        AddMovementInput(Direction, Value);
    }
}

void AScratchCharacter::MoveRight(float Value)
{
    if (ActionState != EActionState::EAS_Unoccupied) return;
    if (Controller && (Value != 0.f))
    {
        // find out which way is right
        const FRotator ControlRotation = GetControlRotation();
        const FRotator YawRotation(0.f, ControlRotation.Yaw, 0.f);
        
        const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
        AddMovementInput(Direction, Value);
    }
}

void AScratchCharacter::Turn(float Value)
{
    AddControllerYawInput(Value);
}

void AScratchCharacter::LookUp(float Value)
{
    AddControllerPitchInput(Value);
}

void AScratchCharacter::EKeyPressed()
{
    AWeapon* OverlappingWeapon = Cast<AWeapon>(OverlappingItem);
    if (OverlappingWeapon)
    {
        OverlappingWeapon->Equip(GetMesh(), FName("RightHandSocket"), this, this);
        CharacterState = ECharacterState::ECS_EquippedOneHandedWeapon;
        OverlappingItem = nullptr;
        EquippedWeapon = OverlappingWeapon;
    }
    else
    {
        if (CanDisarm())
        {
            PlayEquipMontage(FName("Unequip"));
            CharacterState = ECharacterState::ECS_Unequipped;
            ActionState = EActionState::EAS_EquippingWeapon;
        }
        else if (CanArm())
        {
            PlayEquipMontage(FName("Equip"));
            CharacterState = ECharacterState::ECS_EquippedOneHandedWeapon;
            ActionState = EActionState::EAS_EquippingWeapon;
        }
    }
}

void AScratchCharacter::Attack()
{
    Super::Attack();
    const bool bCanAttack =
    ActionState == EActionState::EAS_Unoccupied &&
    CharacterState != ECharacterState::ECS_Unequipped;
    if (bCanAttack)
    {
        PlayAttackMontage();
        ActionState = EActionState::EAS_Attacking;
    }
}

bool AScratchCharacter::CanAttack()
{
    return ActionState == EActionState::EAS_Unoccupied && CharacterState != ECharacterState::ECS_Unequipped;
}

bool AScratchCharacter::CanDisarm()
{
    return ActionState == EActionState::EAS_Unoccupied && CharacterState != ECharacterState::ECS_Unequipped;
}

bool AScratchCharacter::CanArm()
{
    return ActionState == EActionState::EAS_Unoccupied && CharacterState == ECharacterState::ECS_Unequipped && EquippedWeapon;
}

void AScratchCharacter::Disarm()
{
    if(EquippedWeapon)
    {
        EquippedWeapon->AttachMeshToSocket(GetMesh(), FName("SpineSocket"));
    }
}

void AScratchCharacter::Arm()
{
    if(EquippedWeapon)
    {
        EquippedWeapon->AttachMeshToSocket(GetMesh(), FName("RightHandSocket"));
    }
}

void AScratchCharacter::FinishEquipping()
{
    ActionState = EActionState::EAS_Unoccupied;
}

void AScratchCharacter::PlayAttackMontage()
{
    Super::PlayAttackMontage();
    UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
    if (AnimInstance && AttackMontage)
    {
        AnimInstance->Montage_Play(AttackMontage);
        const int32 Selection = FMath::RandRange(0, 1);
        FName SectionName = FName();
        switch (Selection)
        {
            case 0:
                SectionName = FName("Attack1");
                break;
            case 1:
                SectionName = FName("Attack2");
                break;
            default:
                break;
        }
        AnimInstance->Montage_JumpToSection(SectionName, AttackMontage);
    }
}

void AScratchCharacter::PlayEquipMontage(const FName& SectionName)
{
    UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
    if (AnimInstance && EquipMontage)
    {
        AnimInstance->Montage_Play(EquipMontage);
        AnimInstance->Montage_JumpToSection(SectionName, EquipMontage);
    }
}

void AScratchCharacter::AttackEnd()
{
    ActionState = EActionState::EAS_Unoccupied;
    
}

