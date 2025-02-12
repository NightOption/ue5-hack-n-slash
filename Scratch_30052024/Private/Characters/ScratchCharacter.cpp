// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/ScratchCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/AttributeComponent.h"
#include "Items/Item.h"
#include "Items/Weapons/Weapon.h"
#include "Animation/AnimMontage.h"
#include "HUD/ScratchHUD.h"
#include "HUD/ScratchOverlay.h"


AScratchCharacter::AScratchCharacter()
{
	PrimaryActorTick.bCanEverTick = false;
    
    bUseControllerRotationPitch = false;
    bUseControllerRotationYaw = false;
    bUseControllerRotationRoll = false;
    
    GetCharacterMovement()->bOrientRotationToMovement = true;
    GetCharacterMovement()->RotationRate = FRotator(0.f, 300.f, 0.f);
    
    GetMesh()->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
    GetMesh()->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
    GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
    GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Overlap);
    GetMesh()->SetGenerateOverlapEvents(true);

    CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
    CameraBoom->SetupAttachment(GetRootComponent());
    CameraBoom->TargetArmLength = 300.f;
    
    ViewCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("ViewCamera"));
    ViewCamera->SetupAttachment(CameraBoom);
}

void AScratchCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    PlayerInputComponent->BindAxis(FName("MoveForward"), this, &AScratchCharacter::MoveForward);
    PlayerInputComponent->BindAxis(FName("MoveRight"), this, &AScratchCharacter::MoveRight);
    PlayerInputComponent->BindAxis(FName("Turn"), this, &AScratchCharacter::Turn);
    PlayerInputComponent->BindAxis(FName("LookUp"), this, &AScratchCharacter::LookUp);
    
    PlayerInputComponent->BindAction(FName("Jump"), IE_Pressed, this, &AScratchCharacter::Jump);
    PlayerInputComponent->BindAction(FName("Equip"), IE_Pressed, this, &AScratchCharacter::EKeyPressed);
    PlayerInputComponent->BindAction(FName("Attack"), IE_Pressed, this, &AScratchCharacter::Attack);
}

void AScratchCharacter::Jump()
{
    if (IsUnoccupied())
    {
        Super::Jump();
    }
}

float AScratchCharacter::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser)
{
    HandleDamage(DamageAmount);
    SetHUDHealth();
    return DamageAmount;
}

void AScratchCharacter::GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter)
{
    Super::GetHit_Implementation(ImpactPoint, Hitter);
    
    SetWeaponCollisionEnabled(ECollisionEnabled::NoCollision);
    if (Attributes && Attributes->GetHealthPercent() > 0.f)
    {
        ActionState = EActionState::EAS_HitReaction;
    }
}

void AScratchCharacter::SetOverlappingItem(AItem* Item)
{
    OverlappingItem = Item;
}

void AScratchCharacter::AddSouls(ASoul* Soul)
{
    UE_LOG(LogTemp, Warning, TEXT("AScratchCharacter::AdSouls"));
}

void AScratchCharacter::BeginPlay()
{
	Super::BeginPlay();
	
    Tags.Add(FName("EngageableTarget"));
    
    InitializeScratchOverlay();
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
        EquipWeapon(OverlappingWeapon);
    }
    else
    {
        if (CanDisarm())
        {
            Disarm();
        }
        else if (CanArm())
        {
            Arm();
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

void AScratchCharacter::AttackEnd()
{
    ActionState = EActionState::EAS_Unoccupied;
}

void AScratchCharacter::EquipWeapon(AWeapon* Weapon)
{
    Weapon->Equip(GetMesh(), FName("RightHandSocket"), this, this);
    CharacterState = ECharacterState::ECS_EquippedOneHandedWeapon;
    OverlappingItem = nullptr;
    EquippedWeapon = Weapon;
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
    PlayEquipMontage(FName("Unequip"));
    CharacterState = ECharacterState::ECS_Unequipped;
    ActionState = EActionState::EAS_EquippingWeapon;
}

void AScratchCharacter::Arm()
{
    PlayEquipMontage(FName("Equip"));
    CharacterState = ECharacterState::ECS_EquippedOneHandedWeapon;
    ActionState = EActionState::EAS_EquippingWeapon;
}

void AScratchCharacter::AttachWeaponToBack()
{
    if(EquippedWeapon)
    {
        EquippedWeapon->AttachMeshToSocket(GetMesh(), FName("SpineSocket"));
    }
}

void AScratchCharacter::AttachWeaponToHand()
{
    if(EquippedWeapon)
    {
        EquippedWeapon->AttachMeshToSocket(GetMesh(), FName("RightHandSocket"));
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

void AScratchCharacter::Die()
{
    Super::Die();
    
    ActionState = EActionState::EAS_Dead;
    DisableMeshCollision();
}

void AScratchCharacter::FinishEquipping()
{
    ActionState = EActionState::EAS_Unoccupied;
}

void AScratchCharacter::HitReactEnd()
{
    ActionState = EActionState::EAS_Unoccupied;
}

bool AScratchCharacter::IsUnoccupied()
{
    return ActionState == EActionState::EAS_Unoccupied;
}

void AScratchCharacter::InitializeScratchOverlay()
{
    APlayerController* PlayerController = Cast<APlayerController>(GetController());
    if (PlayerController)
    {
        AScratchHUD* ScratchHUD = Cast<AScratchHUD>(PlayerController->GetHUD());
        if (ScratchHUD)
        {
            UScratchOverlay* ScratchOverlay = ScratchHUD->GetScratchOverlay();
            if (ScratchOverlay && Attributes)
            {
                ScratchOverlay->SetHealthBarPercent(Attributes->GetHealthPercent());
                ScratchOverlay->SetStaminaBarPercent(1.f);
                ScratchOverlay->SetGold(0);
                ScratchOverlay->SetSouls(0);
            }
        }
    }
}

void AScratchCharacter::SetHUDHealth()
{
    if (ScratchOverlay && Attributes)
        {
            ScratchOverlay->SetHealthBarPercent(Attributes->GetHealthPercent());
        }
}
