// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/PatrolPath.h"

// Sets default values
APatrolPath::APatrolPath()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

AActor* APatrolPath::GetWaypoint(int32 index) const
{
	if (Waypoints.IsValidIndex(index))
	{
		return Waypoints[index];
	}
	return nullptr;
}

int32 APatrolPath::Num() const
{
	return Waypoints.Num();
}

