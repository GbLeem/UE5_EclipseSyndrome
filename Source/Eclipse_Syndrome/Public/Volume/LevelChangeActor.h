#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LevelChangeActor.generated.h"

class USphereComponent;

UCLASS()
class ECLIPSE_SYNDROME_API ALevelChangeActor : public AActor
{
	GENERATED_BODY()
	
public:	
	ALevelChangeActor();

	UFUNCTION()
	void OnItemOverlap
	(
		UPrimitiveComponent* OverlappedComp
		, AActor* OtherActor
		, UPrimitiveComponent* OtherComp
		, int32 OtherBodyIndex
		, bool bFromSweep
		, const FHitResult& SweepResult
	);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item|Component")
	TObjectPtr<USceneComponent>SceneComp;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item|Component")
	TObjectPtr<USphereComponent> CollisionComp;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item|Component")
	TObjectPtr<UStaticMeshComponent> StaticMeshComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item|Component")
	float CollisionRadius;
};
