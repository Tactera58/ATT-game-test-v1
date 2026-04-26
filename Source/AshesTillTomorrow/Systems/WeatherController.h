#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WeatherController.generated.h"

class UExponentialHeightFogComponent;
class USkyAtmosphereComponent;
class UPostProcessComponent;
class UNiagaraSystem;

UENUM(BlueprintType)
enum class EWeatherType : uint8
{
    Clear    UMETA(DisplayName = "Clear"),
    Rain     UMETA(DisplayName = "Rain"),
    Storm    UMETA(DisplayName = "Storm"),
    Snow     UMETA(DisplayName = "Snow"),
    Fog      UMETA(DisplayName = "Fog"),
    AcidRain UMETA(DisplayName = "Acid Rain")
};

UCLASS()
class ASHESTILLTOMORROW_API AWeatherController : public AActor
{
    GENERATED_BODY()

public:
    AWeatherController();

    virtual void Tick(float DeltaTime) override;

    // ── Components ────────────────────────────────────────────────
    UPROPERTY(VisibleAnywhere, Category = "Components")
    UExponentialHeightFogComponent* FogComponent;

    UPROPERTY(VisibleAnywhere, Category = "Components")
    USkyAtmosphereComponent* SkyAtmosphere;

    UPROPERTY(VisibleAnywhere, Category = "Components")
    UPostProcessComponent* PostProcess;

    // ── Weather Control ───────────────────────────────────────────
    UFUNCTION(BlueprintCallable, Category = "Weather")
    void SetWeatherState(EWeatherType NewWeather);

    UFUNCTION(BlueprintCallable, Category = "Weather")
    EWeatherType GetCurrentWeather() const { return CurrentWeather; }

    UFUNCTION(BlueprintCallable, Category = "Weather")
    void SetTemperature(float Celsius);

    UFUNCTION(BlueprintCallable, Category = "Weather")
    float GetTemperature() const { return CurrentTemperature; }

    // ── Particle FX ───────────────────────────────────────────────
    UPROPERTY(EditAnywhere, Category = "Weather|FX")
    UNiagaraSystem* RainFX;

    UPROPERTY(EditAnywhere, Category = "Weather|FX")
    UNiagaraSystem* SnowFX;

    UPROPERTY(EditAnywhere, Category = "Weather|FX")
    UNiagaraSystem* AcidRainFX;

    // ── Settings ──────────────────────────────────────────────────
    UPROPERTY(EditAnywhere, Category = "Weather")
    float TransitionDuration = 5.0f;

    UPROPERTY(EditAnywhere, Category = "Weather|Hazards")
    float AcidRainDPS = 2.0f;

protected:
    virtual void BeginPlay() override;

private:
    EWeatherType CurrentWeather  = EWeatherType::Clear;
    EWeatherType TargetWeather   = EWeatherType::Clear;

    float CurrentTemperature = 20.0f;
    float CurrentHumidity    = 50.0f;
    float TransitionElapsed  = 0.0f;
    bool  bIsTransitioning   = false;

    void ApplyWeatherImmediate(EWeatherType Weather);
    void UpdateAtmosphericConditions();
    void UpdatePostProcessing();
    void TickTransition(float DeltaTime);
    void TickAcidRainDamage(float DeltaTime);
};