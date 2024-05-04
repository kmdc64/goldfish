// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TP_WeaponComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Weapon.generated.h"

UCLASS()
class PROJECT_GOLDFISH_API AWeapon : public AActor
{
	GENERATED_BODY()

	UPROPERTY(VisibleDefaultsOnly, Category = "Mesh")
	UStaticMeshComponent* m_pMesh;
	
public:
	// Weapon sounds
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
	USoundBase* m_pFireSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
	USoundBase* m_pEmptySound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
	TArray<USoundBase*> m_pImpactSounds;

	// Muzzle offset
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay")
	FVector m_vMuzzleOffset = FVector(100.0f, 0.0f, 10.0f);

	// Muzzle flash
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FX")
	UNiagaraSystem* m_pMuzzleFlash;

	// Sets default values for this actor's properties
	AWeapon();

	// Weapon component
	UTP_WeaponComponent* m_pWeaponComponent;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
