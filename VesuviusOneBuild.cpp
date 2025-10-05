#include "VesuviusOneBuild.h"
#include "NiagaraComponent.h"
#include "NiagaraSystem.h"
#include "Engine/Engine.h"

AVesuviusOneBuild::AVesuviusOneBuild()
{
    PrimaryActorTick.bCanEverTick = true;

    Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
    SetRootComponent(Root);

    PlumeFX = CreateDefaultSubobject<UNiagaraComponent>(TEXT("PlumeFX"));
    PlumeFX->SetupAttachment(Root);
    PlumeFX->bAutoActivate = false; // We'll activate on StartEruption
}

void AVesuviusOneBuild::BeginPlay()
{
    Super::BeginPlay();
    if (PlumeFX && PlumeSystem)
    {
        PlumeFX->SetAsset(PlumeSystem);
    }
}

void AVesuviusOneBuild::Tick(float DeltaTime, ELevelTick TickType, FActorTickFunction& ThisTickFunction)
{
    Super::Tick(DeltaTime, TickType, ThisTickFunction);

    if (!bErupting) return;

    Elapsed += DeltaTime;
    StepSim(DeltaTime);

    if (Elapsed >= Duration)
    {
        StopEruption();
    }
}

void AVesuviusOneBuild::StartEruption(float InDuration)
{
    Duration = InDuration;
    Elapsed = 0.f;
    bErupting = true;

    if (PlumeFX) PlumeFX->Activate(true);
}

void AVesuviusOneBuild::StopEruption()
{
    bErupting = false;
    if (PlumeFX) PlumeFX->Deactivate();
}

void AVesuviusOneBuild::StepSim(float /*Dt*/)
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

void AVesuviusOneBuild::DriveFX(float ExitV, double Mdot)
{
    if (!PlumeFX) return;

    // Map physics -> Niagara user params (create these in your Niagara System)
    const float Spawn = (float)(Mdot * SpawnRateScale); // density
    const float UpVel = ExitV * UpVelocityScale;        // rise speed

    PlumeFX->SetFloatParameter(TEXT("User.SpawnRate"), Spawn);
    PlumeFX->SetFloatParameter(TEXT("User.UpVelocity"), UpVel);
}
