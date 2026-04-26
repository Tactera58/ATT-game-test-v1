#include "Items/Item.h"
#include "Components/StaticMeshComponent.h"

AItem::AItem()
{
    PrimaryActorTick.bCanEverTick = false;

    Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
    RootComponent = Mesh;

    Mesh->SetSimulatePhysics(true);
    Mesh->SetCollisionProfileName(TEXT("PhysicsActor"));
}

void AItem::BeginPlay()
{
    Super::BeginPlay();
}

void AItem::OnPickedUp_Implementation(AActor* Picker)
{
    SetActorHiddenInGame(true);
    Mesh->SetSimulatePhysics(false);
    SetActorEnableCollision(false);
}

void AItem::OnDropped_Implementation(FVector DropLocation)
{
    SetActorLocation(DropLocation);
    SetActorHiddenInGame(false);
    Mesh->SetSimulatePhysics(true);
    SetActorEnableCollision(true);
}