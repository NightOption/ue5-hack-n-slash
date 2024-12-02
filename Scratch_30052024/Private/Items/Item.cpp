// Fill out your copyright notice in the Description page of Project Settings.

#include "Items/Item.h"
#include "Scratch_30052024/DebugMacros.h"
#include "Components/SphereComponent.h"
#include "Characters/ScratchCharacter.h"
#include "NiagaraComponent.h"

AItem::AItem()
{
	PrimaryActorTick.bCanEverTick = true;
    
    ItemMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ItemMeshComponent"));
    RootComponent = ItemMesh;
    
    RotationRate = FRotator(0.f, 45.f, 0.f);
    
    Sphere = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere"));
    Sphere->SetupAttachment(GetRootComponent());
    
    EmbersEffect = CreateDefaultSubobject<UNiagaraComponent>(TEXT("Embers"));
    EmbersEffect->SetupAttachment(GetRootComponent());
}

void AItem::BeginPlay()
{
	Super::BeginPlay();
    
    Sphere->OnComponentBeginOverlap.AddDynamic(this, &AItem::OnSphereOverlap);
    Sphere->OnComponentEndOverlap.AddDynamic(this, &AItem::OnSphereEndOverlap);
}

float AItem::TransformedSin()
{
    return Amplitude * FMath::Sin(RunningTime * TimeConstant);
}

float AItem::TransformedCos()
{
    return Amplitude * FMath::Cos(RunningTime * TimeConstant);
}

void AItem::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult)
{
    AScratchCharacter* ScratchCharacter = Cast<AScratchCharacter>(OtherActor);
    if (ScratchCharacter)
    {
        ScratchCharacter->SetOverlappingItem(this);
    }
}

void AItem::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
    AScratchCharacter* ScratchCharacter = Cast<AScratchCharacter>(OtherActor);
    if (ScratchCharacter)
    {
        ScratchCharacter->SetOverlappingItem(nullptr);
    }
}

void AItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
    
    RunningTime += DeltaTime;
    
    if (ItemState==EItemState::EIS_Hovering)
    {
        float DeltaZ = Amplitude * FMath::Sin(RunningTime * TimeConstant);
        AddActorWorldOffset(FVector(0.f, 0.f, DeltaZ));
        
        FRotator NewRotation = GetActorRotation() + (RotationRate * DeltaTime);
        SetActorRotation(NewRotation);
    }
}
