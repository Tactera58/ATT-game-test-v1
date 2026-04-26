#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Item.generated.h"

UENUM(BlueprintType)
enum class EItemType : uint8
{
    Consumable UMETA(DisplayName = "Consumable"),
    Weapon     UMETA(DisplayName = "Weapon"),
    Armor      UMETA(DisplayName = "Armor"),
    Ammo       UMETA(DisplayName = "Ammo"),
    Resource   UMETA(DisplayName = "Resource"),
    Tool       UMETA(DisplayName = "Tool"),
    Key        UMETA(DisplayName = "Key Item")
};

UCLASS()
class ASHESTILLTOMORROW_API AItem : public AActor
{
    GENERATED_BODY()

public:
    AItem();

    // ── Data ──────────────────────────────────────────────────────
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
    FName ItemID;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
    FText DisplayName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
    FText Description;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
    EItemType ItemType = EItemType::Resource;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
    float WeightKg = 0.1f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
    int32 MaxStackSize = 1;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
    UTexture2D* Icon;

    // ── Interaction ───────────────────────────────────────────────
    /** Called when a player picks this item up */
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Item")
    void OnPickedUp(AActor* Picker);
    virtual void OnPickedUp_Implementation(AActor* Picker);

    /** Called when the item is dropped from inventory */
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Item")
    void OnDropped(FVector DropLocation);
    virtual void OnDropped_Implementation(FVector DropLocation);

protected:
    virtual void BeginPlay() override;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UStaticMeshComponent* Mesh;
};