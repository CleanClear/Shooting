// Fill out your copyright notice in the Description page of Project Settings.


#include "CPP_EnemyBullet.h"
#include "CPP_MyPlayerPawn.h"
#include "CPP_Enemy.h"

ACPP_EnemyBullet::ACPP_EnemyBullet()
{
}

void ACPP_EnemyBullet::OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnComponentBeginOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);

	ACPP_MyPlayerPawn* player = Cast<ACPP_MyPlayerPawn>(OtherActor);
	if (IsValid(player) && player->HitDamage(this))
		Cast<ACPP_Enemy>(GetOwner())->PlayerKill();
}
