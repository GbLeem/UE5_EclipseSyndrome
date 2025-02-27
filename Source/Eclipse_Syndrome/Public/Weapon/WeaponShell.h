#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WeaponShell.generated.h"

UCLASS()
class ECLIPSE_SYNDROME_API AWeaponShell : public AActor
{
	GENERATED_BODY()
	
public:	
	AWeaponShell();
	void EjectShell(FVector Direction);

protected:

public:	

	UPROPERTY(EditDefaultsOnly, Category = "Components")
	UStaticMeshComponent* ShellMesh;
};
