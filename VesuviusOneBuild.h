// Copyright (c) [Your Name/Company]. All rights reserved.
// See Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "VesuviusOneBuild.generated.h"

class UNiagaraComponent;
class UNiagaraSystem;

UCLASS(BlueprintType, Blueprintable)
class VESUVIUSONE_API AVesuviusOneBuild : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AVesuviusOneBuild();
	virtual void Tick(float DeltaTime, ELevelTick TickType, FActorTickFunction& ThisTickFunction) override;

	// Start/stop the eruption
	UFUNCTION(BlueprintCallable)
	void StartEruption(float Duration = 30.f);

	UFUNCTION(BlueprintCallable)
	void StopEruption();

	// (Optional) UI setters for sliders
	UFUNCTION(BlueprintCallable)
	void SetChamberPressure(double Pa) { ChamberPressure_Pa = Pa; }

	UFUNCTION(BlueprintCallable)
	void SetGasFraction(double Phi) { GasFraction = FMath::Clamp(Phi, 0.0, 0.95); }

	UFUNCTION(BlueprintCallable)
	void SetVentRadius(double R) { VentRadius_m = FMath::Max(0.05, R); }

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere)
	USceneComponent* Root;

	// Niagara plume component
	UPROPERTY(VisibleAnywhere)
	UNiagaraComponent* PlumeFX;

	// Assign in Details: your Niagara System asset (e.g., NS_VolcanoPlume)
	UPROPERTY(EditAnywhere, Category = "FX")
	UNiagaraSystem* PlumeSystem;

	// ---- Simulation inputs ----
	UPROPERTY(EditAnywhere, Category = "Sim")
	double ChamberPressure_Pa = 12e6; // 12 MPa

	UPROPERTY(EditAnywhere, Category = "Sim")
	double AtmosphericPressure_Pa = 101325.0;

	UPROPERTY(EditAnywhere, Category = "Sim")
	double GasFraction = 0.25; // phi in [0..1)

	UPROPERTY(EditAnywhere, Category = "Sim")
	double VentRadius_m = 3.0; // meters

	UPROPERTY(EditAnywhere, Category = "Sim")
	double RhoMagma = 2600.0;  // kg/m^3

	UPROPERTY(EditAnywhere, Category = "Sim")
	double GasTemp_K = 1100.0; // K

	// FX mapping (tweak to taste)
	UPROPERTY(EditAnywhere, Category = "FX")
	float SpawnRateScale = 0.02f; // mdot to spawn

	UPROPERTY(EditAnywhere, Category = "FX")
	float UpVelocityScale = 1.0f; // v_exit to velocity

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool bErupting = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float Elapsed = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sim")	
	float Duration = 30.f;

	void StepSim(float Dt);
	void DriveFX(float ExitV, double Mdot);

	// Tiny inline physics helpers
	static double IdealGasDensity(double P, double T_K, double Rspec = 461.5)
	{
		return P / (Rspec * T_K);
	}

	static double MixtureDensity(double RhoMagmaIn, double PhiGas, double RhoGas)
	{
		return (1.0 - PhiGas) * RhoMagmaIn + PhiGas * RhoGas;
	}

	static double ExitVelocity(double P0, double Patm, double RhoMix)
	{
		const double dP = FMath::Max(0.0, P0 - Patm);
		return FMath::Sqrt(2.0 * dP / FMath::Max(1e-6, RhoMix));
	}
};
