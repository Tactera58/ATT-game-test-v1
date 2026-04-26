#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "ATTNPCController.generated.h"

class UBehaviorTree;
class UBlackboardComponent;
class APlayerCharacter;

UENUM(BlueprintType)
enum class ENPCFaction : uint8
{
    Neutral    UMETA(DisplayName = "Neutral"),
    Survivors  UMETA(DisplayName = "Survivors"),
    Raiders    UMETA(DisplayName = "Raiders"),
    Government UMETA(DisplayName = "Government"),
    Infected   UMETA(DisplayName = "Infected")
};

UCLASS()
class ASHESTILLTOMORROW_API AATTNPCController : public AAIController
{
    GENERATED_BODY()

public:
    AATTNPCController();

    // ── AI ────────────────────────────────────────────────────────
    UPROPERTY(EditAnywhere, Category = "AI")
    UBehaviorTree* BehaviorTree;

    UPROPERTY(EditAnywhere, Category = "AI")
    ENPCFaction Faction = ENPCFaction::Neutral;

    /** 0 = fully evil, 100 = fully moral */
    UPROPERTY(EditAnywhere, Category = "AI",
        meta = (ClampMin = "0", ClampMax = "100"))
    int32 MoralityScore = 50;

    // ── Interaction ───────────────────────────────────────────────
    UFUNCTION(BlueprintCallable, Category = "AI|Interaction")
    void BeginConversation(APlayerCharacter* Player);

    UFUNCTION(BlueprintCallable, Category = "AI|Interaction")
    void EndConversation();

    UFUNCTION(BlueprintCallable, Category = "AI|Interaction")
    bool IsHostileTo(APlayerCharacter* Player) const;

    // ── Blackboard Keys ───────────────────────────────────────────
    static const FName BB_PlayerLocation;
    static const FName BB_IsAlert;
    static const FName BB_IsInConversation;
    static const FName BB_MoralityScore;

protected:
    virtual void OnPossess(APawn* InPawn) override;
    virtual void OnUnPossess() override;

private:
    UPROPERTY()
    UBlackboardComponent* BlackboardComponent;

    bool bIsInConversation = false;

    void InitBlackboard();
};