#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Systems/WeatherController.h"
#include "ATTGameMode.generated.h"

UCLASS()
class ASHESTILLTOMORROW_API AATTGameMode : public AGameModeBase
{
    GENERATED_BODY()

public:
    AATTGameMode();

    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;

    // ── Day / Night ───────────────────────────────────────────────
    /** Full day length in real-world seconds. Default: 20 minutes. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "World|Time")
    float DayLengthSeconds = 1200.0f;

    /** 0.0 = midnight, 0.25 = 6AM, 0.5 = noon, 0.75 = 6PM, 1.0 = midnight */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "World|Time")
    float TimeOfDay = 0.25f;

    UFUNCTION(BlueprintCallable, Category = "World|Time")
    float GetHour() const { return TimeOfDay * 24.0f; }

    UFUNCTION(BlueprintCallable, Category = "World|Time")
    bool IsNight() const { return TimeOfDay < 0.2f || TimeOfDay > 0.8f; }

    // ── Weather ───────────────────────────────────────────────────
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "World|Weather")
    AWeatherController* WeatherController;

    /** Min seconds between random weather changes. */
    UPROPERTY(EditAnywhere, Category = "World|Weather")
    float WeatherChangeIntervalMin = 180.0f;

    /** Max seconds between random weather changes. */
    UPROPERTY(EditAnywhere, Category = "World|Weather")
    float WeatherChangeIntervalMax = 600.0f;

    UFUNCTION(BlueprintCallable, Category = "World|Weather")
    void TriggerRandomWeather();

    // ── Player ────────────────────────────────────────────────────
    UFUNCTION(BlueprintCallable, Category = "World")
    void OnPlayerDied(AController* DeadController);

protected:
    virtual void HandleMatchHasStarted() override;

private:
    float WeatherTimer      = 0.0f;
    float NextWeatherChange = 0.0f;

    void TickDayNightCycle(float DeltaTime);
    void TickWeatherScheduler(float DeltaTime);
    void ApplyDayNightToSky();

    EWeatherType SelectRandomWeather() const;
};