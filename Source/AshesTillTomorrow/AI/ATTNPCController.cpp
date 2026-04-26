#include "AI/ATTNPCController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Characters/PlayerCharacter.h"

// Blackboard key names — must match keys in your BT Blackboard asset
const FName AATTNPCController::BB_PlayerLocation   = TEXT("PlayerLocation");
const FName AATTNPCController::BB_IsAlert          = TEXT("IsAlert");
const FName AATTNPCController::BB_IsInConversation = TEXT("IsInConversation");
const FName AATTNPCController::BB_MoralityScore    = TEXT("MoralityScore");

AATTNPCController::AATTNPCController()
{
    BlackboardComponent = CreateDefaultSubobject<UBlackboardComponent>(
        TEXT("BlackboardComponent"));
}

void AATTNPCController::OnPossess(APawn* InPawn)
{
    Super::OnPossess(InPawn);
    InitBlackboard();
}

void AATTNPCController::OnUnPossess()
{
    Super::OnUnPossess();
    StopMovement();
}

// ── Blackboard ────────────────────────────────────────────────────────────────

void AATTNPCController::InitBlackboard()
{
    if (!BehaviorTree)
    {
        UE_LOG(LogTemp, Warning,
            TEXT("ATTNPCController: No BehaviorTree assigned on %s"), *GetName());
        return;
    }

    if (UseBlackboard(BehaviorTree->BlackboardAsset, BlackboardComponent))
    {
        BlackboardComponent->SetValueAsInt(BB_MoralityScore, MoralityScore);
        BlackboardComponent->SetValueAsBool(BB_IsAlert, false);
        BlackboardComponent->SetValueAsBool(BB_IsInConversation, false);

        RunBehaviorTree(BehaviorTree);
    }
}

// ── Interaction ───────────────────────────────────────────────────────────────

void AATTNPCController::BeginConversation(APlayerCharacter* Player)
{
    if (!Player || bIsInConversation) return;
    if (IsHostileTo(Player)) return;

    bIsInConversation = true;
    StopMovement();

    if (BlackboardComponent)
    {
        BlackboardComponent->SetValueAsBool(BB_IsInConversation, true);
    }

    // Face the player
    if (GetPawn())
    {
        FVector ToPlayer = (Player->GetActorLocation()
                         - GetPawn()->GetActorLocation()).GetSafeNormal();
        GetPawn()->SetActorRotation(ToPlayer.Rotation());
    }

    UE_LOG(LogTemp, Log,
        TEXT("ATTNPCController: Conversation started with %s"),
        *GetPawn()->GetName());
}

void AATTNPCController::EndConversation()
{
    bIsInConversation = false;

    if (BlackboardComponent)
    {
        BlackboardComponent->SetValueAsBool(BB_IsInConversation, false);
    }
}

// ── Faction Hostility ─────────────────────────────────────────────────────────

bool AATTNPCController::IsHostileTo(APlayerCharacter* Player) const
{
    if (!Player) return false;

    switch (Faction)
    {
    case ENPCFaction::Raiders:
        return true;

    case ENPCFaction::Infected:
        return true;

    case ENPCFaction::Government:
        // Wire to player reputation system later
        return false;

    case ENPCFaction::Survivors:
    case ENPCFaction::Neutral:
    default:
        return false;
    }
}