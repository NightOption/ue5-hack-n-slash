// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/Enemy.h"
#include "AIController.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Perception/PawnSensingComponent.h"
#include "Components/AttributeComponent.h"
#include "HUD/HealthBarComponent.h"
#include "Navigation/PathFollowingComponent.h"
#include "Items/Weapons/Weapon.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"

#include "Scratch_30052024/DebugMacros.h"

AEnemy::AEnemy()
{
	PrimaryActorTick.bCanEverTick = true;

    GetMesh()->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
    GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
    GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
    GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
    GetMesh()->SetGenerateOverlapEvents(true);
    
    HealthBarWidget = CreateDefaultSubobject<UHealthBarComponent>(TEXT("HealthBar"));
    HealthBarWidget->SetupAttachment(GetRootComponent());
    
    GetCharacterMovement()->bOrientRotationToMovement = true;
    bUseControllerRotationPitch = false;
    bUseControllerRotationYaw = false;
    bUseControllerRotationRoll = false;
    
    PawnSensing = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("PawnSensing"));
    PawnSensing->SightRadius = 4000.f;
    PawnSensing->SetPeripheralVisionAngle(45.f);
}

void AEnemy::PatrolTimerFinished()
{
    MoveToTarget(PatrolTarget);
}

void AEnemy::HideHealthBar()
{
    if (HealthBarWidget)
    {
        HealthBarWidget->SetVisibility(false);
    }
}

void AEnemy::ShowHealthBar()
{
    if (HealthBarWidget)
    {
        HealthBarWidget->SetVisibility(true);
    }
}

void AEnemy::LoseInterst()
{
    CombatTarget = nullptr;
    HideHealthBar();
}

void AEnemy::StartPatrolling()
{
    EnemyState = EEnemyState::EES_Patrolling;
    GetCharacterMovement()->MaxWalkSpeed = PatrollingSpeed;
    MoveToTarget(PatrolTarget);
}

void AEnemy::ChaseTarget()
{
    EnemyState = EEnemyState::EES_Chasing;
    GetCharacterMovement()->MaxWalkSpeed = ChasingSpeed;
    MoveToTarget(CombatTarget);
}

bool AEnemy::IsOutsideCombatRadius()
{
    return !InTargetRange(CombatTarget, CombatRadius);
}

bool AEnemy::IsOutsideAttackRadius()
{
    return !InTargetRange(CombatTarget, AttackRadius);
}

bool AEnemy::IsInsideAttackRadius()
{
    return InTargetRange(CombatTarget, AttackRadius);
}

bool AEnemy:IsChasing()
{
    return EnemyState == EEnemyState::EES_Chasing;
}

bool AEnemy::IsAttacking()
{
    return EnemyState == EEnemyState::EES_Attacking;
}

void AEnemy::BeginPlay()
{
	Super::BeginPlay();
    if (HealthBarWidget)
    {
    HealthBarWidget->SetVisibility(false);
    }
    
    EnemyController = Cast<AAIController>(GetController());
    MoveToTarget(PatrolTarget);
    
    if (PawnSensing)
    {
        PawnSensing->OnSeePawn.AddDynamic(this, &AEnemy::PawnSeen);
    }
    
    UWorld* World = GetWorld();
    if (World && WeaponClass)
    {
        AWeapon* DefaultWeapon = World->SpawnActor<AWeapon>(WeaponClass);
        DefaultWeapon->Equip(GetMesh(), FName("RightHandSocket"), this, this);
        EquippedWeapon = DefaultWeapon;
    }
}

void AEnemy::Die()
{
    UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
    if (AnimInstance && DeathMontage)
    {
        AnimInstance->Montage_Play(DeathMontage);
        
        const int32 Selection = FMath::RandRange(0, 5);
        FName SectionName = FName();
        switch (Selection)
        {
            case 0:
                SectionName = FName("Death1");
                DeathPose = EDeathPose::EDP_Death1;
                break;
            case 1:
                SectionName = FName("Death2");
                DeathPose = EDeathPose::EDP_Death2;
                break;
            case 2:
                SectionName = FName("Death3");
                DeathPose = EDeathPose::EDP_Death3;
                break;
            case 3:
                SectionName = FName("Death4");
                DeathPose = EDeathPose::EDP_Death4;
                break;
            case 4:
                SectionName = FName("Death5");
                DeathPose = EDeathPose::EDP_Death5;
                break;
            case 5:
                SectionName = FName("Death6");
                DeathPose = EDeathPose::EDP_Death6;
                break;
            default:
                break;
        }
        
        AnimInstance->Montage_JumpToSection(SectionName, DeathMontage);
    }
    
    if (HealthBarWidget)
    {
        HealthBarWidget->SetVisibility(false);
    }
    GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    SetLifeSpan(3.f);
}

bool AEnemy::InTargetRange(AActor* Target, double Radius)
{
    if (Target == nullptr) return false;
    const double DistanceToTarget = (Target->GetActorLocation() - GetActorLocation()).Size();
    return DistanceToTarget <= Radius;;
}

void AEnemy::MoveToTarget(AActor* Target)
{
    if (EnemyController == nullptr || Target == nullptr) return;
    FAIMoveRequest MoveRequest;
    MoveRequest.SetGoalActor(Target);
    MoveRequest.SetAcceptanceRadius(60.f);
    EnemyController->MoveTo(MoveRequest);
}

AActor* AEnemy::ChoosePatrolTarget()
{
    TArray<AActor*> ValidTargets;
    for (AActor* Target : PatrolTargets)
    {
        if (Target != PatrolTarget)
        {
            ValidTargets.AddUnique(Target);
        }
    }
    
    const int32 NumPatrolTargets = ValidTargets.Num();
    if (NumPatrolTargets > 0)
    {
        const int32 TargetSelection = FMath::RandRange(0, NumPatrolTargets - 1);
        return ValidTargets[TargetSelection];
    }
    return nullptr;
}

void AEnemy::Attack()
{
    Super::Attack();
    PlayAttackMontage();
}

void AEnemy::PlayAttackMontage()
{
    Super::PlayAttackMontage();
    
    UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
    if (AnimInstance && AttackMontage)
    {
        AnimInstance->Montage_Play(AttackMontage);
        const int32 Selection = FMath::RandRange(0, 2);
        FName SectionName = FName();
        switch (Selection)
        {
            case 0:
                SectionName = FName("Attack1");
                break;
            case 1:
                SectionName = FName("Attack2");
                break;
            case 2:
                SectionName = FName("Attack3");
                break;
            default:
                break;
        }
        AnimInstance->Montage_JumpToSection(SectionName, AttackMontage);
    }
}

void AEnemy::PawnSeen(APawn* SeenPawn)
{
    if (EnemyState == EEnemyState::EES_Chasing) return;
    if (SeenPawn->ActorHasTag(FName("Scratch")))
    {
        GetWorldTimerManager().ClearTimer(PatrolTimer);
        GetCharacterMovement()->MaxWalkSpeed = 300.f;
        CombatTarget = SeenPawn;
        
        if (EnemyState != EEnemyState::EES_Attacking)
                {
                    EnemyState = EEnemyState::EES_Chasing;
                    MoveToTarget(CombatTarget);
                }
    }
}

void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
    
    if (EnemyState > EEnemyState::EES_Patrolling)
    {
        CheckCombatTarget();
    }
    else
    {
        CheckPatrolTarget();
    }
}

void AEnemy::CheckPatrolTarget()
{
    if (InTargetRange(PatrolTarget, PatrolRadius))
    {
        PatrolTarget = ChoosePatrolTarget();
        const float WaitTime = FMath::RandRange(WaitMin, WaitMax);
        GetWorldTimerManager().SetTimer(PatrolTimer, this, &AEnemy::PatrolTimerFinished, WaitTime);
    }
}

void AEnemy::CheckCombatTarget()
{
    if (IsOutsideCombatRadius())
    {
        LoseInterest();
        StartPatrolling();
    }
    else if (IsOutsideAttackRadius() && !IsChasing())
    {
        ChaseTarget();
    }
    else if (IsInsideAttackRadius() && !IsAttacking())
    {
        EnemyState = EEnemyState::EES_Attacking;
        Attack();
    }
}

void AEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void AEnemy::GetHit_Implementation(const FVector& ImpactPoint)
{
//    DRAW_SPHERE_COLOR(ImpactPoint, FColor::Orange);
    if (HealthBarWidget)
    {
        HealthBarWidget->SetVisibility(true);
    }
    if (Attributes && Attributes->IsAlive())
    {
        DirectionalHitReact(ImpactPoint);
    }
    else
    {
        Die();
    }
    
    
    
    if (HitSound)
    {
        UGameplayStatics::PlaySoundAtLocation(
                                              this,
                                              HitSound,
                                              ImpactPoint
                                              );
    }
    if (HitParticles)
    {
        UGameplayStatics::SpawnEmitterAtLocation(
                                                 GetWorld(),
                                                 HitParticles,
                                                 ImpactPoint
                                                 );
    }
}

float AEnemy::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
    if (Attributes)
    {
        Attributes->ReceiveDamage(DamageAmount);
        
        if (HealthBarWidget)
        {
            HealthBarWidget->SetHealthPercent(Attributes->GetHealthPercent());
        }
    }
    CombatTarget = EventInstigator->GetPawn();
    EnemyState = EEnemyState::EES_Chasing;
    GetCharacterMovement()->MaxWalkSpeed = 300.f;
    MoveToTarget(CombatTarget);
    return DamageAmount;
}

void AEnemy::Destroyed()
{
    if (EquippedWeapon)
    {
        EquippedWeapon->Destroy();
    }
}
