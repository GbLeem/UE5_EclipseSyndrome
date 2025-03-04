// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/PatrolPath.h"
#include "Kismet/GameplayStatics.h"
#include "Enemy/EnemyBase.h"

// Sets default values
APatrolPath::APatrolPath()
{

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

