#include "Systems/WeatherController.h"
#include "Components/ExponentialHeightFogComponent.h"
#include "Components/SkyAtmosphereComponent.h"
#include "Components/PostProcessComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Characters/PlayerCharacter.h"

AWeatherController::AWeatherController()
{
    PrimaryActorTick.bCanEverTick = true;

    FogComponent = CreateDefaultSubobject<UExponentialHeightFogComponent>(TEXT("FogComponent"));
    FogComponent->SetupAttachment(RootComponent);

    SkyAtmosphere = CreateDefaultSubobject<USkyAtmosphereComponent>(TEXT("SkyAtmosphere"));
    SkyAtmosphere->SetupAttachment(RootComponent);

    PostProcess = CreateDefaultSubobject<UPostProcessComponent>(TEXT("PostProcess"));
    PostProcess->SetupAttachment(RootComponent);
    PostProcess->BlendRadius = 0.0f;
    PostProcess->BlendWeight  = 1.0f;
}

void AWeatherController::BeginPlay()
{
    Super::BeginPlay();

    // Base post-apocalyptic atmosphere
    FogComponent->FogInscatteringColor = FLinearColor(0.8f, 0.85f, 0.9f);
    ApplyWeatherImmediate(CurrentWeather);
}

void AWeatherController::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (bIsTransitioning)                          TickTransition(DeltaTime);
    if (CurrentWeather == EWeatherType::AcidRain)  TickAcidRainDamage(DeltaTime);
}

// ── Public API ────────────────────────────────────────────────────────────────

void AWeatherController::SetWeatherState(EWeatherType NewWeather)
{
    if (NewWeather == CurrentWeather) return;

    TargetWeather     = NewWeather;
    TransitionElapsed = 0.0f;
    bIsTransitioning  = true;
}

void AWeatherController::SetTemperature(float Celsius)
{
    CurrentTemperature = Celsius;
    UpdateAtmosphericConditions();
}

// ── Weather States ────────────────────────────────────────────────────────────

void AWeatherController::ApplyWeatherImmediate(EWeatherType Weather)
{
    CurrentWeather = Weather;

    switch (Weather)
    {
    case EWeatherType::Clear:
        FogComponent->SetFogDensity(0.005f);
        PostProcess->Settings.bOverride_SceneColorTint = false;
        CurrentHumidity = 30.0f;
        break;

    case EWeatherType::Rain:
        FogComponent->SetFogDensity(0.02f);
        FogComponent->FogHeightFalloff = 0.2f;
        CurrentHumidity = 80.0f;
        if (RainFX)
        {
            UNiagaraFunctionLibrary::SpawnSystemAtLocation(
                GetWorld(), RainFX, GetActorLocation());
        }
        break;

    case EWeatherType::Storm:
        FogComponent->SetFogDensity(0.05f);
        PostProcess->Settings.bOverride_SceneColorTint = true;
        PostProcess->Settings.SceneColorTint = FLinearColor(0.75f, 0.75f, 1.0f);
        CurrentHumidity = 95.0f;
        break;

    case EWeatherType::Snow:
        FogComponent->SetFogDensity(0.03f);
        FogComponent->FogInscatteringColor = FLinearColor(0.9f, 0.95f, 1.0f);
        CurrentHumidity = 60.0f;
        if (SnowFX)
        {
            UNiagaraFunctionLibrary::SpawnSystemAtLocation(
                GetWorld(), SnowFX, GetActorLocation());
        }
        break;

    case EWeatherType::Fog:
        FogComponent->SetFogDensity(0.1f);
        FogComponent->FogHeightFalloff = 0.05f;
        CurrentHumidity = 85.0f;
        break;

    case EWeatherType::AcidRain:
        FogComponent->SetFogDensity(0.04f);
        FogComponent->FogInscatteringColor = FLinearColor(0.6f, 0.8f, 0.4f);
        PostProcess->Settings.bOverride_SceneColorTint = true;
        PostProcess->Settings.SceneColorTint = FLinearColor(0.85f, 1.0f, 0.7f);
        if (AcidRainFX)
        {
            UNiagaraFunctionLibrary::SpawnSystemAtLocation(
                GetWorld(), AcidRainFX, GetActorLocation());
        }
        break;
    }

    UpdateAtmosphericConditions();
    UpdatePostProcessing();
}

// ── Internal Ticks ────────────────────────────────────────────────────────────

void AWeatherController::TickTransition(float DeltaTime)
{
    TransitionElapsed += DeltaTime;
    if (TransitionElapsed >= TransitionDuration)
    {
        bIsTransitioning = false;
        ApplyWeatherImmediate(TargetWeather);
    }
}

void AWeatherController::TickAcidRainDamage(float DeltaTime)
{
    TArray<AActor*> Players;
    UGameplayStatics::GetAllActorsOfClass(
        GetWorld(), APlayerCharacter::StaticClass(), Players);

    for (AActor* Actor : Players)
    {
        APlayerCharacter* PC = Cast<APlayerCharacter>(Actor);
        if (PC && PC->SurvivalComponent)
        {
            PC->SurvivalComponent->ApplyDamage(AcidRainDPS * DeltaTime);
        }
    }
}

void AWeatherController::UpdateAtmosphericConditions()
{
    // Drive wetness parameter on all Environment-tagged actors
    TArray<AActor*> EnvActors;
    UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName("Environment"), EnvActors);

    for (AActor* Actor : EnvActors)
    {
        TArray<UStaticMeshComponent*> Meshes;
        Actor->GetComponents<UStaticMeshComponent>(Meshes);

        for (UStaticMeshComponent* Mesh : Meshes)
        {
            if (UMaterialInstanceDynamic* DynMat = Mesh->CreateDynamicMaterialInstance(0))
            {
                DynMat->SetScalarParameterValue("Wetness", CurrentHumidity / 100.0f);
            }
        }
    }
}

void AWeatherController::UpdatePostProcessing()
{
    FPostProcessSettings& PP = PostProcess->Settings;

    PP.AntiAliasingMethod           = EAntiAliasingMethod::AAM_TemporalAA;
    PP.TemporalAASamples            = 8;
    PP.TemporalAACurrentFrameWeight = 0.2f;

    PP.AutoExposureMethod        = EAutoExposureMethod::AEM_Histogram;
    PP.AutoExposureMinBrightness = FMath::Lerp(0.3f, 1.2f, CurrentHumidity / 100.0f);
}