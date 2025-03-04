#include "Volume/Time_TriggerVolimeActor.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/World.h"
#include "TimerManager.h"

ATime_TriggerVolimeActor::ATime_TriggerVolimeActor()
{
	PrimaryActorTick.bCanEverTick = false;

	TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
	RootComponent = TriggerBox;

	TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &ATime_TriggerVolimeActor::OnOverlapBegin);
}

void ATime_TriggerVolimeActor::BeginPlay()
{
	Super::BeginPlay();
}

void ATime_TriggerVolimeActor::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                              UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
                                              bool bFromSweep, const FHitResult& SweepResult)

{
    if (OtherActor && TargetMesh)
    {

        TargetMesh->SetSimulatePhysics(true);

        // 2초 후 메시 삭제
        FTimerHandle TimerHandle;
        GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &ATime_TriggerVolimeActor::RemoveMesh, 3.0f, false);
    }
}

void ATime_TriggerVolimeActor::RemoveMesh()
{
    if (TargetMesh)
    {
        TargetMesh->DestroyComponent();
    }
}