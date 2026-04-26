#include "Characters/PlayerCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"

APlayerCharacter::APlayerCharacter()
{
    PrimaryActorTick.bCanEverTick = true;

    SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
    SpringArm->SetupAttachment(RootComponent);
    SpringArm->TargetArmLength = 0.0f;
    SpringArm->bUsePawnControlRotation = true;

    FirstPersonCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
    FirstPersonCamera->SetupAttachment(SpringArm);
    FirstPersonCamera->bUsePawnControlRotation = false;

    SurvivalComponent = CreateDefaultSubobject<USurvivalComponent>(TEXT("SurvivalComponent"));
}

void APlayerCharacter::BeginPlay()
{
    Super::BeginPlay();
    GetCharacterMovement()->MaxWalkSpeed = BaseWalkSpeed;
}

void APlayerCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    UpdateMovementState();
}

// ── Input ─────────────────────────────────────────────────────────────────────

void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    PlayerInputComponent->BindAxis("MoveForward", this, &APlayerCharacter::MoveForward);
    PlayerInputComponent->BindAxis("MoveRight",   this, &APlayerCharacter::MoveRight);

    PlayerInputComponent->BindAction("Sprint",    IE_Pressed,  this, &APlayerCharacter::StartSprinting);
    PlayerInputComponent->BindAction("Sprint",    IE_Released, this, &APlayerCharacter::StopSprinting);
    PlayerInputComponent->BindAction("Crouch",    IE_Pressed,  this, &APlayerCharacter::StartCrouching);
    PlayerInputComponent->BindAction("Crouch",    IE_Released, this, &APlayerCharacter::StopCrouching);
    PlayerInputComponent->BindAction("Slide",     IE_Pressed,  this, &APlayerCharacter::StartSliding);
    PlayerInputComponent->BindAction("Mantle",    IE_Pressed,  this, &APlayerCharacter::TryMantle);
    PlayerInputComponent->BindAction("Jump",      IE_Pressed,  this, &ACharacter::Jump);
    PlayerInputComponent->BindAction("Jump",      IE_Released, this, &ACharacter::StopJumping);
    PlayerInputComponent->BindAction("Attack",    IE_Pressed,  this, &APlayerCharacter::BasicAttack);
    PlayerInputComponent->BindAction("AltAttack", IE_Pressed,  this, &APlayerCharacter::SpecialAttack);
    PlayerInputComponent->BindAction("Aim",       IE_Pressed,  this, &APlayerCharacter::StartAiming);
    PlayerInputComponent->BindAction("Aim",       IE_Released, this, &APlayerCharacter::StopAiming);
    PlayerInputComponent->BindAction("Interact",  IE_Pressed,  this, &APlayerCharacter::Interact);
}

// ── Movement ──────────────────────────────────────────────────────────────────

void APlayerCharacter::MoveForward(float Value)
{
    if (Controller && Value != 0.0f)
    {
        const FRotator YawRotation(0, Controller->GetControlRotation().Yaw, 0);
        AddMovementInput(FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X), Value);
    }
}

void APlayerCharacter::MoveRight(float Value)
{
    if (Controller && Value != 0.0f)
    {
        const FRotator YawRotation(0, Controller->GetControlRotation().Yaw, 0);
        AddMovementInput(FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y), Value);
    }
}

void APlayerCharacter::StartSprinting()
{
    if (bIsCrouching || bIsSliding) return;
    if (SurvivalComponent->GetStamina() <= 0.0f) return;

    bIsSprinting = true;
    GetCharacterMovement()->MaxWalkSpeed = BaseWalkSpeed * SprintSpeedMultiplier;
    CurrentMovementState = EMovementState::Sprinting;
}

void APlayerCharacter::StopSprinting()
{
    if (!bIsSprinting) return;
    bIsSprinting = false;
    GetCharacterMovement()->MaxWalkSpeed = BaseWalkSpeed;
}

void APlayerCharacter::StartCrouching()
{
    bIsCrouching = true;
    Crouch();
    CurrentMovementState = EMovementState::Crouching;
}

void APlayerCharacter::StopCrouching()
{
    bIsCrouching = false;
    UnCrouch();
}

void APlayerCharacter::StartSliding()
{
    if (!bIsCrouching || bIsSliding) return;
    bIsSliding = true;
    GetCharacterMovement()->MaxWalkSpeed *= SlideSpeedMultiplier;
    CurrentMovementState = EMovementState::Sliding;

    FTimerHandle SlideTimer;
    GetWorldTimerManager().SetTimer(
        SlideTimer, this, &APlayerCharacter::StopSliding, 1.2f, false);
}

void APlayerCharacter::StopSliding()
{
    if (!bIsSliding) return;
    bIsSliding = false;
    GetCharacterMovement()->MaxWalkSpeed /= SlideSpeedMultiplier;
}

void APlayerCharacter::TryMantle()
{
    FVector Start = GetActorLocation();
    FVector End   = Start + GetActorForwardVector() * 80.0f;

    FHitResult Hit;
    FCollisionQueryParams Params;
    Params.AddIgnoredActor(this);

    if (GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, Params))
    {
        float HeightDiff = Hit.ImpactPoint.Z - Start.Z;
        if (HeightDiff > 0.0f && HeightDiff <= MantleHeightThreshold)
        {
            Mantle(Hit.ImpactPoint);
        }
    }
}

void APlayerCharacter::Mantle(FVector MantleTarget)
{
    CurrentMovementState = EMovementState::Mantling;
    FVector Direction = (MantleTarget - GetActorLocation()).GetSafeNormal();
    LaunchCharacter(Direction * 500.0f, false, true);
}

void APlayerCharacter::UpdateMovementState()
{
    if (bIsSliding)   { CurrentMovementState = EMovementState::Sliding;   return; }
    if (bIsSprinting) { CurrentMovementState = EMovementState::Sprinting; return; }
    if (bIsCrouching) { CurrentMovementState = EMovementState::Crouching; return; }

    float Speed = GetVelocity().Size();
    CurrentMovementState = (Speed > 10.0f) ?