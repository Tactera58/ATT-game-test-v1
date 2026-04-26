#include "Systems/CraftingComponent.h"
#include "Items/Item.h"
#include "Engine/World.h"

UCraftingComponent::UCraftingComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void UCraftingComponent::BeginPlay()
{
    Super::BeginPlay();
}

// ── Crafting ──────────────────────────────────────────────────────────────────

bool UCraftingComponent::CraftItem(FName RecipeName, bool bAtCraftingStation)
{
    const FCraftingRecipe* Recipe = FindRecipe(RecipeName);
    if (!Recipe)
    {
        UE_LOG(LogTemp, Warning,
            TEXT("CraftingComponent: Recipe '%s' not found."), *RecipeName.ToString());
        OnCraftFail.Broadcast(RecipeName);
        return false;
    }

    if (Recipe->bRequiresCraftingStation && !bAtCraftingStation)
    {
        UE_LOG(LogTemp, Warning,
            TEXT("CraftingComponent: '%s' requires a crafting station."),
            *RecipeName.ToString());
        OnCraftFail.Broadcast(RecipeName);
        return false;
    }

    if (!HasMaterials(*Recipe))
    {
        OnCraftFail.Broadcast(RecipeName);
        return false;
    }

    ConsumeMaterials(*Recipe);
    SpawnResult(*Recipe);
    OnCraftSuccess.Broadcast(RecipeName);
    return true;
}

bool UCraftingComponent::CanCraft(FName RecipeName, bool bAtCraftingStation) const
{
    const FCraftingRecipe* Recipe = FindRecipe(RecipeName);
    if (!Recipe) return false;
    if (Recipe->bRequiresCraftingStation && !bAtCraftingStation) return false;
    return HasMaterials(*Recipe);
}

// ── Inventory ─────────────────────────────────────────────────────────────────

void UCraftingComponent::AddItem(FName ItemID, int32 Quantity)
{
    if (Quantity <= 0) return;
    Inventory.FindOrAdd(ItemID) += Quantity;
}

bool UCraftingComponent::RemoveItem(FName ItemID, int32 Quantity)
{
    int32* Held = Inventory.Find(ItemID);
    if (!Held || *Held < Quantity) return false;

    *Held -= Quantity;
    if (*Held <= 0) Inventory.Remove(ItemID);
    return true;
}

int32 UCraftingComponent::GetItemCount(FName ItemID) const
{
    const int32* Held = Inventory.Find(ItemID);
    return Held ? *Held : 0;
}

// ── Internal ──────────────────────────────────────────────────────────────────

const FCraftingRecipe* UCraftingComponent::FindRecipe(FName RecipeName) const
{
    if (!RecipeDataTable) return nullptr;
    return RecipeDataTable->FindRow<FCraftingRecipe>(
        RecipeName, TEXT("CraftingComponent::FindRecipe"));
}

bool UCraftingComponent::HasMaterials(const FCraftingRecipe& Recipe) const
{
    for (const FIngredient& Ingredient : Recipe.RequiredMaterials)
    {
        if (GetItemCount(Ingredient.ItemID) < Ingredient.Quantity)
        {
            return false;
        }
    }
    return true;
}

void UCraftingComponent::ConsumeMaterials(const FCraftingRecipe& Recipe)
{
    for (const FIngredient& Ingredient : Recipe.RequiredMaterials)
    {
        RemoveItem(Ingredient.ItemID, Ingredient.Quantity);
    }
}

void UCraftingComponent::SpawnResult(const FCraftingRecipe& Recipe)
{
    if (!Recipe.ResultItemClass) return;

    FVector SpawnLoc = GetOwner()->GetActorLocation()
                     + GetOwner()->GetActorForwardVector() * 80.0f;

    FActorSpawnParameters SpawnParams;
    SpawnParams.SpawnCollisionHandlingOverride =
        ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

    for (int32 i = 0; i < Recipe.ResultQuantity; ++i)
    {
        GetWorld()->SpawnActor<AItem>(
            Recipe.ResultItemClass, SpawnLoc, FRotator::ZeroRotator, SpawnParams);
    }
}