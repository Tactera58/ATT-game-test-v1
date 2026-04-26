#include "Systems/SurvivalComponent.h"

USurvivalComponent::USurvivalComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
}

void USurvivalComponent::BeginPlay()
{
    Super::BeginPlay();

    Health  = MaxHealth;
    Hunger  = MaxHunger;
    Thirst  = MaxThirst;
    Stamina = MaxStamina;
}

void USurvivalComponent::TickComponent(float DeltaTime, ELevelTick TickType,
    FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    if (bIsDead) return;

    TickHunger(DeltaTime);
    TickThirst(DeltaTime);
    TickStamina(DeltaTime);
    CheckCriticalStats();
}

// ── Hunger ────────────────────────────────────────────────────────────────────

void USurvivalComponent::TickHunger(float DeltaTime)
{
    Hunger = FMath::Clamp(Hunger - HungerDepletionRate * DeltaTime, 0.0f, MaxHunger);
    OnHungerChanged.Broadcast(Hunger);
}

// ── Thirst ────────────────────────────────────────────────────────────────────

void USurvivalComponent::TickThirst(float DeltaTime)
{
    Thirst = FMath::Clamp(Thirst - ThirstDepletionRate * DeltaTime, 0.0f, MaxThirst);
    OnThirstChanged.Broadcast(Thirst);
}

// ── Stamina — ATT formula: OSDRV = (BRV × EM) - CWD ─────────────────────────

void USurvivalComponent::TickStamina(float DeltaTime)
{
    float Rate;

    if (bIsAiming)
    {
        // Full weapon + armor drain while ADS
        float WeaponCWD = (CurrentWeapon.WeightKg / ReferenceWeaponKg)
                        * ReferenceWeaponDrain
                        * CurrentWeapon.ConditionFactor
                        * CurrentWeapon.SkillReductionFactor;

        float ArmorCWD  = (CurrentArmor.WeightKg / ReferenceArmorKg)
                        * ReferenceArmorDrain
                        * CurrentArmor.ConditionFactor
                        * CurrentArmor.SkillReductionFactor;

        Rate = (BaseStaminaRegenRate * CurrentWeapon.ErgonomicsMultiplier)
               - WeaponCWD - ArmorCWD;
    }
    else
    {
        // Passive regen — only armor weight applies when not aiming
        float ArmorCWD = (CurrentArmor.WeightKg / ReferenceArmorKg)
                       * ReferenceArmorDrain
                       * CurrentArmor.ConditionFactor
                       * CurrentArmor.SkillReductionFactor;

        Rate = BaseStaminaRegenRate - ArmorCWD;
    }

    Stamina = FMath::Clamp(Stamina + Rate * DeltaTime, 0.0f, MaxStamina);
    OnStaminaChanged.Broadcast(Stamina);
}

float USurvivalComponent::CalculateStaminaRate(const FStaminaEquipmentParams& Params) const
{
    float AdjustedWeight = Params.WeightKg * Params.ConditionFactor;
    float CWD = (AdjustedWeight / ReferenceWeaponKg)
              * ReferenceWeaponDrain
              * Params.SkillReductionFactor;

    // OSDRV = (BRV × EM) - CWD
    return (BaseStaminaRegenRate * Params.ErgonomicsMultiplier) - CWD;
}

void USurvivalComponent::SetAiming(bool bAiming)
{
    bIsAiming = bAiming;
}

void USurvivalComponent::SetEquipmentParams(const FStaminaEquipmentParams& WeaponParams,
                                             const FStaminaEquipmentParams& ArmorParams)
{
    CurrentWeapon = WeaponParams;
    CurrentArmor  = ArmorParams;
}

// ── Health ────────────────────────────────────────────────────────────────────

void USurvivalComponent::ApplyDamage(float Amount)
{
    if (bIsDead) return;
    Health = FMath::Clamp(Health - Amount, 0.0f, MaxHealth);
    OnHealthChanged.Broadcast(Health);
}

void USurvivalComponent::Heal(float Amount)
{
    Health = FMath::Clamp(Health + Amount, 0.0f, MaxHealth);
    OnHealthChanged.Broadcast(Health);
}

void USurvivalComponent::ConsumeFood(float HungerRestored, float HealthBonus)
{
    Hunger = FMath::Clamp(Hunger + HungerRestored, 0.0f, MaxHunger);
    OnHungerChanged.Broadcast(Hunger);
    if (HealthBonus > 0.0f) Heal(HealthBonus);
}

void USurvivalComponent::ConsumeWater(float ThirstRestored)
{
    Thirst = FMath::Clamp(Thirst + ThirstRestored, 0.0f, MaxThirst);
    OnThirstChanged.Broadcast(Thirst);
}

// ── Critical Checks ───────────────────────────────────────────────────────────

void USurvivalComponent::CheckCriticalStats()
{
    // Starvation or dehydration bleeds HP at 0.2/sec
    if (Hunger <= 0.0f || Thirst <= 0.0f)
    {
        ApplyDamage(0.2f);
    }

    if (Health <= 0.0f && !bIsDead)
    {
        bIsDead = true;
        OnDeath.Broadcast();
    }
}