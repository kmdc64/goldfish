// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon.h"

// Sets default values
AWeapon::AWeapon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	m_pMesh = CreateDefaultSubobject<UStaticMeshComponent>("Weapon Mesh");
	m_pWeaponComponent = CreateDefaultSubobject<UTP_WeaponComponent>("Weapon Component");
	m_pMesh->SetupAttachment(m_pWeaponComponent);
}

// Called when the game starts or when spawned
void AWeapon::BeginPlay()
{
	Super::BeginPlay();
	
	// Assign weapon component variables
	m_pWeaponComponent->m_pFireSound = m_pFireSound;
	m_pWeaponComponent->m_pEmptySound = m_pEmptySound;
	m_pWeaponComponent->m_pImpactSounds = m_pImpactSounds;
	m_pWeaponComponent->m_vMuzzleOffset = m_vMuzzleOffset;
	m_pWeaponComponent->m_pMuzzleFlash = m_pMuzzleFlash;
}

// Called every frame
void AWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

