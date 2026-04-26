#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "InteractInterface.generated.h"

UINTERFACE(MinimalAPI, BlueprintType)
class UInteractInterface : public UInterface
{
    GENERATED_BODY()
};

/**
 * IInteractInterface
 *
 * Implement this on any actor the player can interact with:
 * items, doors, terminals, crafting stations, NPCs, etc.
 *
 * Usage in your actor:
 *   .h  — class AMyActor : public AActor, public IInteractInterface
 *   .cpp — void AMyActor::OnInteract_Implementation(AActor* Interactor) { ... }
 */
class ASHESTILLTOMORROW_API IInteractInterface
{
    GENERATED_BODY()

public:
    /** Called when the player presses interact while looking at this actor. */
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
    void OnInteract(AActor* Interactor);

    /** Text shown in the HUD when the player looks at this actor. */
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
    FText GetInteractPrompt() const;
};