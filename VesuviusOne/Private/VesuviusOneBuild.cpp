#include "VesuviusOneBuild.h"
#include "NiagaraComponent.h"
#include "NiagaraSystem.h"
#include "Engine/Engine.h"

void VesuviusOneBuild::DriveFX(float ExitV, double Mdot)
{
    if (!PlumeFX) return;

    // Map your physics to parameters
    const float Spawn = (float)(Mdot * SpawnRateScale);
    const float UpVel = ExitV * UpVelocityScale;

    // These must match your Niagara User params exactly
    PlumeFX->SetFloatParameter(TEXT("User.SpawnRate"), Spawn);
    PlumeFX->SetFloatParameter(TEXT("User.Velocity"), UpVel);  // or "User.UpVelocity" if that's the name you made
}

VesuviusOneBuild::VesuviusOneBuild()
{
    PrimaryActorTick.bCanEverTick = true;

    Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
    SetRootComponent(Root);

    PlumeFX = CreateDefaultSubobject<UNiagaraComponent>(TEXT("PlumeFX"));
    PlumeFX->SetupAttachment(Root);
    PlumeFX->bAutoActivate = false; // we'll activate on StartEruption
}

void VesuviusOneBuild::BeginPlay()
{
    Super::BeginPlay();
    if (PlumeSystem) PlumeFX->SetAsset(PlumeSystem);
}

void VesuviusOneBuild::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (!bErupting) return;

    Elapsed += DeltaTime;
    StepSim(DeltaTime);

    if (Elapsed >= Duration)
    {
        StopEruption();
    }
}

void VesuviusOneBuild::StartEruption(float InDuration)
{
    Duration = InDuration;
    Elapsed = 0.f;
    bErupting = true;

    if (PlumeFX) PlumeFX->Activate(true);
}

void VesuviusOneBuild::StopEruption()
{
    bErupting = false;
    if (PlumeFX) PlumeFX->Deactivate();
}

void VesuviusOneBuild::StepSim(float /*Dt*/)
{
    // --- Simple pressure-driven model ---
    const double RhoGas = IdealGasDensity(ChamberPressure_Pa, GasTemp_K);      // ideal gas
    const double RhoMix = MixtureDensity(RhoMagma, GasFraction, RhoGas);       // magma + gas
    const double Vexit = ExitVelocity(ChamberPressure_Pa, AtmosphericPressure_Pa, RhoMix);
    const double Area = PI * VentRadius_m * VentRadius_m;                    // m^2
    const double Mdot = RhoMix * Area * Vexit;                               // kg/s

    // HUD debug
    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(
            /*Key*/1, /*Time*/0.f, FColor::Orange,
            FString::Printf(TEXT("φ=%.2f  r=%.1fm  ρmix=%.0f  v=%.0f m/s  ṁ=%.0f kg/s"),
                GasFraction, VentRadius_m, RhoMix, Vexit, Mdot));
    }

    DriveFX((float)Vexit, Mdot);
}

void VesuviusOneBuild::DriveFX(float ExitV, double Mdot)
{
    if (!PlumeFX) return;

    // Map physics -> Niagara user params (create these in your Niagara System)
    const float Spawn = (float)(Mdot * SpawnRateScale); // density
    const float UpVel = ExitV * UpVelocityScale;        // rise speed

    PlumeFX->SetFloatParameter(TEXT("User.SpawnRate"), Spawn);
    PlumeFX->SetFloatParameter(TEXT("User.UpVelocity"), UpVel);
}
