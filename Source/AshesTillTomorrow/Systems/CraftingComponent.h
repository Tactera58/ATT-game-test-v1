#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Engine/DataTable.h"
#include "CraftingComponent.generated.h"

class AItem;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCraftSuccess, FName, RecipeName);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCraftFail,    FName, RecipeName);

USTRUCT(BlueprintType)
struct FIngredient
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName ItemID;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "1"))
    int32 Quantity = 1;
};

USTRUCT(BlueprintType)
struct FCraftingRecipe : public FTableRowBase
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FText DisplayName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<FIngredient> RequiredMaterials;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TSubclassOf<AItem> ResultItemClass;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "1"))
    int32 ResultQuantity = 1;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bRequiresCraftingStation = false;
};

UCLASS(ClassGroup=(AshesTillTomorrow), meta=(BlueprintSpawnableComponent))
class ASHESTILLTOMORROW_API UCraftingComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UCraftingComponent();

    // ── Delegates ─────────────────────────────────────────────────
    UPROPERTY(BlueprintAssignable, Category = "Crafting|Events")
    FOnCraftSuccess OnCraftSuccess;

    UPROPERTY(BlueprintAssignable, Category = "Crafting|Events")
    FOnCraftFail OnCraftFail;

    // ── Crafting ──────────────────────────────────────────────────
    UFUNCTION(BlueprintCallable, Category = "Crafting")
    bool CraftItem(FName RecipeName, bool bAtCraftingStation = false);

    UFUNCTION(BlueprintCallable, Category = "Crafting")
    bool CanCraft(FName RecipeName, bool bAtCraftingStation = false) const;

    // ── Inventory ─────────────────────────────────────────────────
    UFUNCTION(BlueprintCallable, Category = "Crafting|Inventory")
    void AddItem(FName ItemID, int32 Quantity = 1);

    UFUNCTION(BlueprintCallable, Category = "Crafting|Inventory")
    bool RemoveItem(FName ItemID, int32 Quantity = 1);

    UFUNCTION(BlueprintCallable, Category = "Crafting|Inventory")
    int32 GetItemCount(FName ItemID) const;

    UFUNCTION(BlueprintCallable, Category = "Crafting|Inventory")
    TMap<FName, int32> GetInventory() const { return Inventory; }

    // ── Data ──────────────────────────────────────────────────────
    UPROPERTY(EditAnywhere, Category = "Crafting")
    UDataTable* RecipeDataTable;

protected:
    virtual void BeginPlay() override;

private:
    UPROPERTY(VisibleAnywhere, Category = "Crafting|Inventory")
    TMap<FName, int32> Inventory;

    const FCraftingRecipe* FindRecipe(FName RecipeName) const;
    bool HasMaterials(const FCraftingRecipe& Recipe) const;
    void ConsumeMaterials(const FCraftingRecipe& Recipe);
    void SpawnResult(const FCraftingRecipe& Recipe);
};