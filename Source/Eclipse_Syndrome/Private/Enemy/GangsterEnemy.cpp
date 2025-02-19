// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/GangsterEnemy.h"

// Sets default values
AGangsterEnemy::AGangsterEnemy()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AGangsterEnemy::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AGangsterEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AGangsterEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

