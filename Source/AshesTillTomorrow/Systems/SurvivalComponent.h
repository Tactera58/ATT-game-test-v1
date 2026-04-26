#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SurvivalComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnStatChanged, float, NewValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDeath);

USTRUCT(BlueprintType)
struct FStaminaEquipmentParams
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stamina")
    float WeightKg = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stamina",
        meta = (ClampMin = "0.0", ClampMax = "1.0"))
    float ErgonomicsMultiplier = 1.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stamina",
        meta = (ClampMin = "0.0", ClampMax = "1.0"))
    float SkillReductionFactor = 1.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stamina",
        meta = (ClampMin = "0.0", ClampMax = "1.0"))
    float ConditionFactor = 1.0f;
};

UCLASS(ClassGroup=(AshesTillTomorrow), meta=(BlueprintSpawnableComponent))
class ASHESTILLTOMORROW_API USurvivalComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    USurvivalComponent();

    virtual void TickComponent(float DeltaTime, ELevelTick TickType,
        FActorComponentTickFunction* ThisTickFunction) override;

    // Delegates
    UPROPERTY(BlueprintAssignable, Category = "Survival|Events")
    FOnStatChanged OnHealthChanged;

    UPROPERTY(BlueprintAssignable, Category = "Survival|Events")
    FOnStatChanged OnHungerChanged;

    UPROPERTY(BlueprintAssignable, Category = "Survival|Events")
    FOnStatChanged OnThirstChanged;

    UPROPERTY(BlueprintAssignable, Category = "Survival|Events")
    FOnStatChanged OnStaminaChanged;

    UPROPERTY(BlueprintAssignable, Category = "Survival|Events")
    FOnDeath OnDeath;

    // Getters
    UFUNCTION(BlueprintCallable, Category = "Survival")
    float GetHealth() const { return Health; }

    UFUNCTION(BlueprintCallable, Category = "Survival")
    float GetHunger() const { return Hunger; }

    UFUNCTION(BlueprintCallable, Category = "Survival")
    float GetThirst() const { return Thirst; }

    UFUNCTION(BlueprintCallable, Category = "Survival")
    float GetStamina() const { return Stamina; }

    UFUNCTION(BlueprintCallable, Category = "Survival")
    float GetHealthPercent() const { return Health / MaxHealth; }

    UFUNCTION(BlueprintCallable, Category = "Survival")
    float GetStaminaPercent() const { return Stamina / MaxStamina; }

    // Modifiers
    UFUNCTION(BlueprintCallable, Category = "Survival")
    void ApplyDamage(float Amount);

    UFUNCTION(BlueprintCallable, Category = "Survival")
    void Heal(float Amount);

    UFUNCTION(BlueprintCallable, Category = "Survival")
    void ConsumeFood(float HungerRestored, float HealthBonus = 0.0f);

    UFUNCTION(BlueprintCallable, Category = "Survival")
    void ConsumeWater(float ThirstRestored);

    // Stamina / Equipment
    UFUNCTION(BlueprintCallable, Category = "Survival|Stamina")
    void SetEquipmentParams(const FStaminaEquipmentParams& WeaponParams,
                            const FStaminaEquipmentParams& ArmorParams);

    UFUNCTION(BlueprintCallable, Category = "Survival|Stamina")
    float CalculateStaminaRate(const FStaminaEquipmentParams& Params) const;

    UFUNCTION(BlueprintCallable, Category = "Survival|Stamina")
    void SetAiming(bool bAiming);

protected:
    virtual void BeginPlay() override;

    // Max values
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Survival|Stats")
    float MaxHealth = 100.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Survival|Stats")
    float MaxHunger = 100.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Survival|Stats")
    float MaxThirst = 100.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Survival|Stats")
    float MaxStamina = 100.0f;

    // Depletion rates per second
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Survival|Rates")
    float HungerDepletionRate = 0.5f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Survival|Rates")
    float ThirstDepletionRate = 0.8f;

    // ATT stamina formula: BRV = 1.25/sec
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Survival|Rates")
    float BaseStaminaRegenRate = 1.25f;

    UPROPERTY(EditAnywhere, Category = "Survival|StaminaFormula")
    float ReferenceWeaponKg = 4.267f;

    UPROPERTY(EditAnywhere, Category = "Survival|StaminaFormula")
    float ReferenceWeaponDrain = 1.0f;

    UPROPERTY(EditAnywhere, Category = "Survival|StaminaFormula")
    float ReferenceArmorKg = 10.0f;

    UPROPERTY(EditAnywhere, Category = "Survival|StaminaFormula")
    float ReferenceArmorDrain = 2.0f;

private:
    float Health;
    float Hunger;
    float Thirst;
    float Stamina;

    bool bIsAiming = false;
    bool bIsDead   = false;

    FStaminaEquipmentParams CurrentWeapon;
    FStaminaEquipmentParams CurrentArmor;

    void TickHunger(float DeltaTime);
    void TickThirst(float DeltaTime);
    void TickStamina(float DeltaTime);
    void CheckCriticalStats();
};